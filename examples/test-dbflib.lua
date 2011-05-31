--[[
Copyright (c) 2011 Daniel Minor 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
--]]

dbflib = require('dbflib')

-- create a .dbf file and add some fields 
dbf = dbflib.create('test.dbf')
dbf:addfield('name', dbflib.String, 8, 0)
dbf:addfield('value', dbflib.Double, 8, 2)

-- add some data
dbf:writefield(1, 1, 'test')
dbf:writefield(1, 2, '2.0')
dbf:writefield(2, 1, 'test 2')
dbf:writefield(2, 2, 2.0)

-- close the .dbf file
dbf:close()

-- read the .dbf file and write out the data
dbf = dbflib.open('test.dbf')
print('field count: ' .. dbf:fieldcount())
print('record count: ' .. dbf:recordcount())
print()

-- field info 
for field = 1, dbf:fieldcount() do
    t, n, w, d = dbf:fieldinfo(field)
    print('field: ' .. field)
    print('name: ' .. n)
    print('type: ' .. t)
    print('width: ' .. w)
    print('decimals: ' .. d)
    print()
end

for record = 1, dbf:recordcount() do
    for field = 1, dbf:fieldcount() do
        print(dbf:readfield(record, field))
    end
end

dbf:close()
