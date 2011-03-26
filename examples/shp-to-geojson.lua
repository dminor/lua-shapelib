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

require('shapelib')

if #arg ~= 2 then 
    print("usage: shp-to-geojson <shp> <out>")
    os.exit(1) 
end

local shp = shapelib.open(arg[1], 'rb')
local ofile = assert(io.open(arg[2], 'w'))

local entcount
local shptype
entcount, shptype = shapelib.getinfo(shp)

ofile:write([[{ "type": "GeometryCollection",
  "geometries": [
]])

for i=1,entcount do

    local o = shp[i]

    if shptype == shapelib.Point then 
        ofile:write('    { "type": "Point",\n')
    elseif shptype == shapelib.Arc then
        ofile:write('    { "type": "LineString",\n') 
    elseif shptype == shapelib.Polygon then
        ofile:write('    { "type": "Polygon",\n') 
    end

    ofile:write('      "coordinates": [\n')
    for v=1,#o do
        ofile:write('        [' .. o[v].x .. ',' .. o[v].y .. ']')
        if v == #o then
            ofile:write('\n')
        else
            ofile:write(',\n') 
        end
    end 
    ofile:write('      ]\n')

    if i == entcount then
        ofile:write('    }\n')
    else
        ofile:write('    },\n')
    end 
end

ofile:write('  ]\n')
ofile:write('}\n')
ofile:close()

shapelib.close(shp) 
