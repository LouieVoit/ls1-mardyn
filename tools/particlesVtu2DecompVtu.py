#!/usr/bin/python3

import re
import sys
import os.path
import multiprocessing

"""
This script takes a list of pvtu files containing particle positions,
generated by ls1 mardyn and converts them to pvtu and their corresponding
vtu files describing the domain decomposition.
"""



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
        if '<DataArray Name="points"' in line:
            # split off xml tags, split all numbers, and convert to floats
            splitLine = line.replace('<','>').split('>')[2].split(' ')
            # Special case: this vtu file contains no particles
            if splitLine == ['\n']:
                # return something that does not drive paraview crazy
                return [(0.,0.), (0.,0.), (0.,0.)]

            data = list(map(float, splitLine))          # this conversion is the script's hotspot
            # extract min and max of every dimension
            minMaxCoord=[(min(data[dim::3]), max(data[dim::3])) for dim in range(3)]
            break

    if not minMaxCoord:
       filename=particlesVtu.name
       raise RuntimeError(f"Could not find any DataArray of points or positions in {filename}")
    return minMaxCoord

def writeVoxelPVtu(filename, minMaxCoords, vtuFiles):

    """Write the pvtu file that references all vtu files of this timestep

    Args:
        filename: Name of the file to be written.
        minMaxCoords: Total bounding box. [(xMin, xMax), (yMin, yMax), (zMin, zMax)]
        vtuFiles: List of files that should be referenced.
    """

    with open(filename, "w") as f:
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

    """Write the vtu file that describes one rank box.

    Args:
        filename: Name of the file to be written.
        minMaxCoords: Coordinates of the box to be written. [(xMin, xMax), (yMin, yMax), (zMin, zMax)]
        rank: This file's rank number.
    """

    with open(filename, "w") as f:
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

    """Wrap all steps to convert a particle vtu to a voxel vtu

    Args:
        particleFilePath: Path to the existing particle vtu.
        voxelFilePath: Path (incl. name) to the voxel file that will be written.

    """

    basename = os.path.basename(particleFilePath)
    try:
        rank = re.search('node([0-9]+)', basename).group(1)
    except AttributeError:
        print(f"Could not find node number in {basename}")
        raise

    with open(particleFilePath, 'r') as leFile:
        coords=particleMinMax(leFile)
    writeVoxelVtu(voxelFilePath, coords, rank)
    return coords

def processPVtu(particleFilePath, outputDir):

    """Wrap all steps to convert a particle pvtu and all its referenced vtu to voxel (p)vtu.

    Args:
        particleFilePath: Path to the existing particle pvtu.

    """

    newFilePrefix = "decomp_"

    particleFileAbspath = os.path.abspath(particleFilePath)
    absdirname = os.path.dirname(particleFileAbspath)
    particleFileBasename = os.path.basename(particleFilePath)
    outputPVtuAbs = os.path.join(outputDir, newFilePrefix + particleFileBasename)
    print(f"{particleFilePath} -> {os.path.relpath(outputPVtuAbs)}")

    with open(particleFilePath, 'r') as f:
        # extent of all referenced files [(xMin, xMax), (yMin, yMax), (zMin, zMax)]
        globalExtent = [(float("inf"), float("-inf")),
                  (float("inf"), float("-inf")),
                  (float("inf"), float("-inf"))]
        # list of filenames to write to the pvtu
        vtuFiles = []
        # find all sub files
        for line in f:
            match = re.search('\s*<Piece.*Source="([^"]+)"', line)
            if match:
                subFileNameRel = match.group(1)
                subFileNameAbs = os.path.normpath(os.path.join(absdirname, subFileNameRel))
                voxelFilePath = os.path.join(outputDir, newFilePrefix + os.path.basename(subFileNameAbs))
                vtuExtent = processVtu(subFileNameAbs, voxelFilePath)
                globalExtent = [(min(globalExtent[0][0], vtuExtent[0][0]), max(globalExtent[0][1], vtuExtent[0][1])),
                                (min(globalExtent[1][0], vtuExtent[1][0]), max(globalExtent[1][1], vtuExtent[1][1])),
                                (min(globalExtent[2][0], vtuExtent[2][0]), max(globalExtent[2][1], vtuExtent[2][1]))]
                vtuFiles.append(os.path.basename(voxelFilePath))
        writeVoxelPVtu(outputPVtuAbs, globalExtent, vtuFiles)

############################################ Script ############################################
if __name__ == '__main__':
    # Require at least three argument.
    if len(sys.argv) < 3:
        raise RuntimeError(f"""\
Not enough input arguments.

Usage: {os.path.basename(sys.argv[0])} outputDir files.pvtu...
""")

    outputDir = sys.argv[1]
    # Create the output dir if it doesn't exist yet
    os.makedirs(outputDir, exist_ok=True)
    files = sys.argv[2:]

    def task(filepath):
        _, extension = os.path.splitext(filepath)
        if extension == ".pvtu":
            processPVtu(filepath, outputDir)
        else:
            raise RuntimeError(f"Unknown file extension {extension} in\n{filepath}")

    # process each pvtu with all its pieces individually but in parallel
    multiprocessing.Pool().map(task, files)

