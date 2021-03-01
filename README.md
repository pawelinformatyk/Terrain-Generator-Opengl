# Terrain-Generator-Opengl

List of content 
- [Setting up](#Settingupagridofvertices)
- [Rendering](#Renderingmesh)
- [Shaders](#Shaders)
- [Usage](#Programcontrols)

## Setting up a grid of vertices
	
Methods of generating vertices
- from texture (heightmap) 
![heightmap](/readme_res/terrHeightMap.png)
- from txt file - format x,z,y(height)
![text file](/readme_res/terrTxtFile.png)
- from perlin noise, random 
![random](/readme_res/terrRandom.png)

## Rendering mesh

Render terrain using one triangle strip. To create one strip for mesh I need to link strips by using degenerate triangles between them.

## Shaders

For shaders I use Phong shading with Blinn-Phong reflection model.

- Display model with diffuse and specular map.
![texture](/readme_res/texture.png)
- Diplay every vertice of terrain.
![texture](/readme_res/points.png)
- Display normals, which are created in geometry shader. For each vertice line is created displaying normal vector.
![normals](/readme_res/normals.png)
- Display gradient with step 
![step](/readme_res/step.png)
- Display gradient
![gradient](/readme_res/gradient.png)
- Display custom water and terrain
![terrainwater](/readme_res/terrainwater.png)

## Program controls

- display modes 
	- 1 model with texture and lighting 
	- 2 vertices of model 
	- 3 model with texture and its normals
	- 4 model with color changing depending on height with steps
	- 5 model with color changing depending on height 
	- 6 model with custom water and terrain
- rotate, zoom with mouse
- qa - change height of light source
- wsad - change position of light source
- [] increase scale 
- space = start/stop animation 