#!/usr/bin/python3

import re
import sys
import os.path


def particleMinMax(particlesVtu):

    """Find min/max particle coordinates of a given file
    Args:
        particlesVtu: file handle that shall be parsed

    Note:
        md-flex writes particle positions line by line and in an data array called "positions",
        which needs to be parsed differently.
    """

    minMaxCoord=[]

    for line in particlesVtu:
        # find the line with all coordinates
        # Format x0 y0 z0 x1 y1 z1 ...
        if re.search('\s*<DataArray Name="points"', line):
            # split off xml tags, split all numbers, and convert to floats
            data=[float(i) for i in line.replace('<','>').split('>')[2].split(' ')]
            # extract min and max of every dimension
            minMaxCoord=[(min(data[dim::3]), max(data[dim::3])) for dim in range(3)]
            break

    if not minMaxCoord:
       filename=particlesVtu.name
       raise RuntimeError(f"Could not find any DataArray of points or positions in {filename}")
    return minMaxCoord

def writeVoxelPVtu(filename, minMaxCoords, vtuFiles):
    f = open(filename, "w")
    f.write(f"""\
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<VTKFile byte_order="LittleEndian" type="PUnstructuredGrid" version="0.1">
  <PUnstructuredGrid>
    <PPointData/>
    <PCellData>
      <PDataArray type="Int32" Name="Rank" />
    </PCellData>
    <PPoints>
      <DataArray NumberOfComponents="3" format="ascii" type="Float32">
        {minMaxCoords[0][0]} {minMaxCoords[1][0]} {minMaxCoords[2][0]}
        {minMaxCoords[0][1]} {minMaxCoords[1][0]} {minMaxCoords[2][0]}
        {minMaxCoords[0][0]} {minMaxCoords[1][1]} {minMaxCoords[2][0]}
        {minMaxCoords[0][1]} {minMaxCoords[1][1]} {minMaxCoords[2][0]}
        {minMaxCoords[0][0]} {minMaxCoords[1][0]} {minMaxCoords[2][1]}
        {minMaxCoords[0][1]} {minMaxCoords[1][0]} {minMaxCoords[2][1]}
        {minMaxCoords[0][0]} {minMaxCoords[1][1]} {minMaxCoords[2][1]}
        {minMaxCoords[0][1]} {minMaxCoords[1][1]} {minMaxCoords[2][1]}
      </DataArray>
    </PPoints>
""")
    for vtuFile in vtuFiles:
        f.write(f"""\
    <Piece Source="{vtuFile}"/>
""")
    f.write(f"""\
  </PUnstructuredGrid>
</VTKFile>
""")

def writeVoxelVtu(filename, minMaxCoords, rank):
    f = open(filename, "w")
    f.write(f"""\
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<VTKFile byte_order="LittleEndian" type="UnstructuredGrid" version="0.1">
    <UnstructuredGrid>
        <Piece NumberOfPoints="8" NumberOfCells="1"> 
            <PointData>
            </PointData>
            <CellData>
                <DataArray type="Int32" Name="Rank" format="ascii">
                  {rank}
                </DataArray>
            </CellData>
            <Points>
                <DataArray type="Float32" NumberOfComponents="3" format="ascii">
                    {minMaxCoords[0][0]} {minMaxCoords[1][0]} {minMaxCoords[2][0]}
                    {minMaxCoords[0][1]} {minMaxCoords[1][0]} {minMaxCoords[2][0]}
                    {minMaxCoords[0][0]} {minMaxCoords[1][1]} {minMaxCoords[2][0]}
                    {minMaxCoords[0][1]} {minMaxCoords[1][1]} {minMaxCoords[2][0]}
                    {minMaxCoords[0][0]} {minMaxCoords[1][0]} {minMaxCoords[2][1]}
                    {minMaxCoords[0][1]} {minMaxCoords[1][0]} {minMaxCoords[2][1]}
                    {minMaxCoords[0][0]} {minMaxCoords[1][1]} {minMaxCoords[2][1]}
                    {minMaxCoords[0][1]} {minMaxCoords[1][1]} {minMaxCoords[2][1]}
                </DataArray>
            </Points>
            <Cells> 
                <DataArray type="Int32" Name="connectivity" format="ascii">
                    0 1 2 3 4 5 6 7
                </DataArray> 
                <DataArray type="Int32" Name="offsets" format="ascii">
                8 
                </DataArray> 
                <DataArray type="Int32" Name="types" format="ascii"> 
                11 
                </DataArray> 
            </Cells>
        </Piece> 
    </UnstructuredGrid> 
</VTKFile>
""")

def processVtu(particleFilePath, voxelFilePath):
    basename = os.path.basename(particleFilePath)
    try:
        rank = re.search('node([0-9]+)', basename).group(1)
    except AttributeError:
        print(f"Could not find node number in {basename}")
        raise

    leFile = open(particleFilePath, 'r')
    coords=particleMinMax(leFile)
    writeVoxelVtu(voxelFilePath, coords, rank)
    return coords

def processPVtu(particleFilePath):
    abspath = os.path.abspath(particleFilePath)
    absdirname = os.path.dirname(abspath)
    basename = os.path.basename(particleFilePath)

    f = open(particleFilePath, 'r')
    # extent of all referenced files [(xMin, xMax), (yMin, yMax), (zMin, zMax)]
    extent = [(float("inf"),float("-inf")),
              (float("inf"),float("-inf")),
              (float("inf"),float("-inf"))]
    vtuFiles = []
    for line in f:
        match = re.search('\s*<Piece.*Source="([^"]+)"', line)
        if match:
            vtuFileNameRel = match.group(1)
            vtuFileName = os.path.normpath(absdirname + "/" + vtuFileNameRel)
            voxelFilePath = os.path.dirname(vtuFileName) + "/decomp_" + os.path.basename(vtuFileName)
            vtuExtent = processVtu(vtuFileName, voxelFilePath)
            extent = [(min(extent[0][0], vtuExtent[0][0]), max(extent[0][1], vtuExtent[0][1])),
                      (min(extent[1][0], vtuExtent[1][0]), max(extent[1][1], vtuExtent[1][1])),
                      (min(extent[2][0], vtuExtent[2][0]), max(extent[2][1], vtuExtent[2][1]))]
            vtuFiles.append(os.path.relpath(voxelFilePath))
    writeVoxelPVtu(absdirname + "/decomp_" + basename, extent, vtuFiles)

############################################ Script ############################################
if __name__ == '__main__':


    if len(sys.argv) < 2:
        print(f"""\
Not enough input arguments.

Usage: {os.path.basename(sys.argv[0])} files.pvtu...
""")

    # process each pvtu with all its pieces individually
    for filepath in sys.argv[1:]:
        _, extension = os.path.splitext(filepath)
        if extension == ".pvtu":
            processPVtu(filepath)
        else:
            raise RuntimeError(f"Unknown file extension {extension} in\n{filepath}")

