# Terrain-Generator-Opengl

The terrain is built as a grid of vertices elevated with y-coordinate from txt file, which contains coordinates in sequance x,z,y.  
To draw model I used index buffet object and linking together triangle strips with degenerate triangles for highest performance.

![Model with lighting](/readme_res/default.png)

![Model with texture](/readme_res/texture.png)

![Model with color depending on height](/readme_res/height.png)

![Model wireframe](/readme_res/wireframe.png)
