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
        ofile:write('        [' .. o[v].x .. ',' .. o[v].y .. '],\n')
    end 
    ofile:write('      ],\n')
    ofile:write('    },\n')
end

ofile:write('  ]\n')
ofile:write('}\n')
ofile:close()


