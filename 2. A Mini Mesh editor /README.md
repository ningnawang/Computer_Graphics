# A Mini Mesh Editor
<http://462cmu.github.io/asst2_meshedit/>

- Implemented a simple mesh editor to manipulate and resample triangle meshes
- Developed a tool to load and edit COLLADA mesh files used by major modeling packages and graphics engines

## Implementation: <br/>
- <strong>Upsampling</strong> <br/>
- <strong>Downsampling</strong> <br/>
- <strong>Resampling</strong> <br/>
- <strong>Edge Flip</strong> <br/>
- <strong>Edge Split</strong> <br/>
- <strong>Edge Collapse</strong> <br/>


## Build Instructions
- Make sure you have <strong>cmake</strong> installed
- Open Terminal
- Under "1. A Mini-SVG Renderer" file, run following command:
- $ <strong> mkdir build && cd build && cmake .. && make && make </strong>
- Then you can change following path to rasterize every file under <strong> /dae/ </strong> directory:

### example 1
- $  <strong> ./meshedit ../dae/cow.dae </strong>
- press <strong> U </strong> for upsampling 
![alt tag](https://github.com/junanita/Computer-Graphics/blob/master/2.%20A%20Mini%20Mesh%20editor%20/result/upsampling.png)

### example 2
- $  <strong> ./meshedit ../dae/cow.dae </strong>
- press <strong> D </strong> for downsampling 
![alt tag](https://github.com/junanita/Computer-Graphics/blob/master/2.%20A%20Mini%20Mesh%20editor%20/result/downsampling.png)

### example 3
- $  <strong> ./meshedit ../dae/cow.dae </strong>
- press <strong> R </strong> for resampling 
![alt tag](https://github.com/junanita/Computer-Graphics/blob/master/2.%20A%20Mini%20Mesh%20editor%20/result/resampling.png)
