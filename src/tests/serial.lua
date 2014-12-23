----------------------------------------
-- serial.lua
----------------------------------------
local serial = {}
local visited
function serialize(v, d)
	visited = {}
	return serial[type(v)](v, d)
end
setmetatable(serial, {__call = function(_, v) return serialize(v) end})

serial.number  = tostring
serial.boolean = tostring
serial['nil']  = tostring
serial.string  = function(s)
	return ('%q'):format(s)
end
serial.table = function(t, d)
	if visited[t] then return '#REC_REF' end
	if next(t) == nil then return '{}' end
	d = d and d+1 or 1
	local r = {nil, nil, nil, nil}
	for i, v in ipairs(t) do
		r[#r+1] = serialize(v, d) .. ','
	end
	for k, v in pairs(t) do
		if not tonumber(k) or k > #t then
			-- index out of ipairs range?
			if type(k) ~= 'string' or not k:match '^[_%a][_%w]*$' then
				-- enclose keys in square brackets
				k = ('[%s]'):format(serialize(k, d))
			end
			if #k > 7 and #k%2 == 0 then k = k .. ' ' end
			r[#r+1] = ('%-20s = %s,'):format(k, serialize(v, d))
		end
	end
	r[#r+1] = ''
	visited[t] = true
	return '{\n'..('  '):rep(d)..table.concat(r,'\n'..('  '):rep(d)):sub(1, -3) .. '}'
end

local fn_index = setmetatable({}, {__mode='k'})
-- Weak table to be able to unobstruct the gc
for _, lib in ipairs {
	'coroutine','debug','io','math','os',
	'package','string','table', '_G'
} do
	for k, fn in pairs(_G[lib]) do
		if type(fn)=='function' and
			not pcall(string.dump, fn) then
				k = (lib=='_G') and k or (lib..'.'..k)
				fn_index[fn] = k
		end
	end
end
serial['function'] = function(f)
	if fn_index[f] then return fn_index[f] end
	if true then return tostring(f) end
	local ok, s = pcall(string.dump, f)
	if ok then return s end
	error 'Cannot serialize given function'
end
serial['thread'] = function()
	error 'Cannot serialize type "thread"'
end
serial['userdata'] = tostring

return serial