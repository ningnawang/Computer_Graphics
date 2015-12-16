# A Mini-SVG Renderer
<http://462cmu.github.io/asst1_drawsvg/>

- Implemented a simple software rasterizer that draws points, lines, triangles, and bitmap images.
- A viewer that supports the basic features of the Scalable Vector Graphics (SVG) format that is now widely used on the internet.

## Implementation: <br/>
- <strong>Supersampling</strong> <br/>
- <strong>Transformation</strong> <br/>
- <strong>Texture Mapping</strong> <br/>
- <strong>Mipmap</strong> <br/>
- <strong>Occlusion</strong> <br/>
- <strong>Alpha Compositing</strong> <br/>


## Build Instructions
- Make sure you have <strong>cmake</strong> installed
- Open Terminal
- Under "1. A Mini-SVG Renderer" file, run following command:
- $ <strong> mkdir build && cd build && cmake ../src && make && make install </strong>
- Then you can change following path to rasterize every file under <strong> /svg/ </strong> directory:

- $  <strong> ./drawsvg ../svg/basic/test3.svg </strong>
![alt tag](https://github.com/junanita/Computer-Graphics/blob/master/1.%20A%20Mini-SVG%20Renderer%20/result/04_scotty_result.png)

- $  <strong> ./drawsvg ../svg/alpha/04_scotty.svg </strong>
![alt tag](https://github.com/junanita/Computer-Graphics/blob/master/1.%20A%20Mini-SVG%20Renderer%20/result/test3_result.png)

