
# Lua Termview

A small library for accesing the terminal from Lua


### Library functions

* `termview.new(x, y, w, h)`     	- Creates a new window
* `termview.beep()`              	- Beep
* `termview.flash()`             	- Flashes the screen
* `termview.flush()`             	- Flushes the screen
* `termview.defColor(i, r, g, b)`	- Defines a new color
* `termview.resizeWindow(w, h)`  	- Resizes the terminal window

### View functions

* view:x()                                 	- Returns the x position
* view:y()                                 	- Returns the y position
* view:w()                                 	- Returns the width
* view:h()                                 	- Returns the height
* view:move(x, y)                          	- Moves the view
* view:resize(w, h)                        	- Resizes the view
* view:setStyle(fg_col, bg_col, vid_mode)  	- Sets the current text style
* view:setBGStyle(fg_col, bg_col, vid_mode)	- Sets the background style
* view:show()                              	- Shows a previously hidden view
* view:hide()                              	- Hides a view
* view:isHidden()                          	- Returns true if it's hidden
* view:toTop()                             	- Moves the view to the top of the stack
* view:toBottom()                          	- Moves the view to the bottom of the stack
* view:curX()                              	- Returns the cursor's x position
* view:curY()                              	- Returns the cursor's y position
* view:getCursor()                         	- Returns the cursor's position
* view:setCursor(x, y)                     	- Sets the cursor's position
* view:moveCursor(dx, dy)                  	- Moves the cursor by the specified ammount
* view:border()                            	- Draws a border on the view
* view:vline(x, y, h)                      	- Draws a vertical line
* view:hline(x, y, w)                      	- Draws a horizontal line
* view:touch()                             	- Forces a redraw of the view
* view:clear()                             	- Clears the view
* view:clearLine(i)                        	- Clears the ith line
* view:setScroll(scroll)                   	- Sets if the view can scroll
* view:scroll(i)                           	- Scrolls the view by i lines
* view:copy(src, sx, sy, dx, dy, dw, dh)   	- Copies from the `src` to the `dst` view
* view:getc()                              	- Reads a character from `stdin`
* view:gets(max_len)                       	- Reads a string from `stdin`
* view:insc(char)                          	- Inserts a character
* view:putc(char)                          	- Writes a character
* view:puts(str)                           	- Writes a string
* view:printf(str, ...)                    	- Writes a string (works like `string.format`)