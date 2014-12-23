print 'Inside Lua'

if not view then
	package.cpath = '../lib?.so'
	view = require 'termview'
end

serial = require 'serial' -- must be global

view.start()

_G.console = view.new(0, 0, 0, 0)
console:setScroll(true)
console:setBGStyle('white', 'black')

local function console_assert(expr, msg)
	if not expr then
		console:setStyle('red', 'black')
		console:puts(msg .. '\n\r')
		console:setStyle()
	end
	return expr
end

local type_color = {
	['string']  	= 'green',
	['number']  	= 'red',
	['table']   	= 'blue',
	['function']	= 'yellow',
	['nil']     	= 'red',
}

local function print_val(val)
	local col = type_color[type(val)]
	if col then console:setStyle(col, 'black') end
	console:puts(serial(val))
	if col then console:setStyle() end
end

local function print_ret(...)
	local len = select('#', ...)
	if len ~= 0 or ... ~= nil then
		console:puts('< ')
		for i=1, len do
			print_val(select(i, ...))
			if i < len then console:puts(', ') end
		end
		console:puts('\n')
	end
end

function print(...)
	local len = select('#', ...)
	for i=1, len do
		console:puts(tostring(select(i, ...)))
		if i < len then console:putc '\t' end
	end
	console:puts('\n')
end

while true do
	console:setStyle('blue', 'black')
	console:puts("> ")
	console:setStyle()
	view.flush()
	local str = console:gets(128)
	if str == 'quit' or str == 'exit' then break end
	local chunk = console_assert(loadstring('return ' .. str, 'input'))
	if chunk then
		local ok, err = pcall(function()
			return print_ret(chunk())
		end)
		if not ok then
			console_assert(false, err and debug.traceback(err) or debug.traceback())
		end
	end
end

view.finish()