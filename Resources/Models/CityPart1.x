xof 0303txt 0032
template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}

template AnimTicksPerSecond {
 <9e415a43-7ba6-4a73-8743-b73d47e88476>
 DWORD AnimTicksPerSecond;
}


AnimTicksPerSecond {
 24;
}

Mesh  {
 52;
 70.722206;71.482391;52.355179;,
 70.722206;71.482384;-51.547768;,
 73.357529;71.482391;54.588070;,
 73.357529;71.482384;-53.794918;,
 73.357529;0.000000;-53.794918;,
 -74.555313;0.000000;-53.794918;,
 -74.555313;71.482384;-53.794918;,
 73.357529;0.000000;54.588070;,
 -74.555313;0.000000;54.588070;,
 -74.555313;4.962054;52.355179;,
 -74.555313;71.482391;54.588070;,
 -74.555313;71.482391;52.355179;,
 -74.555313;4.962054;-51.547768;,
 70.722206;4.962054;52.355179;,
 70.722206;4.962054;-51.547768;,
 -74.555313;71.482384;-51.547768;,
 70.722206;71.482391;52.355179;,
 70.722206;71.482391;52.355179;,
 70.722206;71.482391;52.355179;,
 70.722206;71.482384;-51.547768;,
 70.722206;71.482384;-51.547768;,
 73.357529;71.482391;54.588070;,
 73.357529;71.482391;54.588070;,
 73.357529;71.482391;54.588070;,
 73.357529;71.482384;-53.794918;,
 73.357529;71.482384;-53.794918;,
 73.357529;0.000000;-53.794918;,
 73.357529;0.000000;-53.794918;,
 -74.555313;0.000000;-53.794918;,
 -74.555313;0.000000;-53.794918;,
 -74.555313;71.482384;-53.794918;,
 -74.555313;71.482384;-53.794918;,
 73.357529;0.000000;54.588070;,
 73.357529;0.000000;54.588070;,
 -74.555313;0.000000;54.588070;,
 -74.555313;0.000000;54.588070;,
 -74.555313;0.000000;54.588070;,
 -74.555313;4.962054;52.355179;,
 -74.555313;4.962054;52.355179;,
 -74.555313;4.962054;52.355179;,
 -74.555313;71.482391;54.588070;,
 -74.555313;71.482391;54.588070;,
 -74.555313;71.482391;52.355179;,
 -74.555313;71.482391;52.355179;,
 -74.555313;4.962054;-51.547768;,
 -74.555313;4.962054;-51.547768;,
 70.722206;4.962054;52.355179;,
 70.722206;4.962054;52.355179;,
 70.722206;4.962054;-51.547768;,
 70.722206;4.962054;-51.547768;,
 -74.555313;71.482384;-51.547768;,
 -74.555313;71.482384;-51.547768;;
 28;
 3;18,23,1;,
 3;1,23,3;,
 3;27,29,25;,
 3;25,29,31;,
 3;33,36,26;,
 3;26,36,28;,
 3;35,41,39;,
 3;39,41,43;,
 3;32,22,34;,
 3;34,22,40;,
 3;21,7,24;,
 3;24,7,4;,
 3;38,47,45;,
 3;45,47,49;,
 3;17,46,42;,
 3;42,46,37;,
 3;51,44,20;,
 3;20,44,48;,
 3;14,13,19;,
 3;19,13,16;,
 3;8,9,5;,
 3;5,9,12;,
 3;5,12,30;,
 3;30,12,50;,
 3;11,10,0;,
 3;0,10,2;,
 3;3,6,1;,
 3;1,6,15;;

 MeshNormals {
  52;
  0.000000;1.000000;-0.000001;,
  0.000000;1.000000;0.000001;,
  0.000000;1.000000;-0.000001;,
  0.000000;1.000000;0.000002;,
  1.000000;0.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;1.000000;0.000001;,
  1.000000;0.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;1.000000;-0.000001;,
  0.000000;1.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  -0.707107;0.000000;-0.707107;,
  -1.000000;0.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  -0.707107;0.000000;-0.707107;,
  -0.707107;0.000000;-0.707107;,
  0.000000;1.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  1.000000;0.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;1.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;1.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;1.000000;0.000000;,
  -0.707107;0.000000;-0.707107;,
  0.000000;1.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;1.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;0.000000;1.000000;;
  28;
  3;18,23,1;,
  3;1,23,3;,
  3;27,29,25;,
  3;25,29,31;,
  3;33,36,26;,
  3;26,36,28;,
  3;35,41,39;,
  3;39,41,43;,
  3;32,22,34;,
  3;34,22,40;,
  3;21,7,24;,
  3;24,7,4;,
  3;38,47,45;,
  3;45,47,49;,
  3;17,46,42;,
  3;42,46,37;,
  3;51,44,20;,
  3;20,44,48;,
  3;14,13,19;,
  3;19,13,16;,
  3;8,9,5;,
  3;5,9,12;,
  3;5,12,30;,
  3;30,12,50;,
  3;11,10,0;,
  3;0,10,2;,
  3;3,6,1;,
  3;1,6,15;;
 }

 MeshTextureCoords {
  52;
  0.813559;-0.700083;,
  0.792473;-0.714746;,
  0.813915;-0.699935;,
  0.827423;-0.700369;,
  0.847391;-0.685992;,
  0.787487;-0.685992;,
  0.807455;-0.700369;,
  0.827423;-0.685992;,
  0.807455;-0.685992;,
  0.807043;-0.693181;,
  0.793947;-0.699935;,
  0.793947;-0.700083;,
  0.807455;-0.700220;,
  0.531477;-0.031008;,
  0.034391;-0.032199;,
  0.807455;-0.700220;,
  0.531339;-0.451308;,
  0.026178;-0.924100;,
  0.827067;-0.693329;,
  0.033262;-0.451989;,
  0.615194;-0.925362;,
  0.827423;-0.693181;,
  0.827423;-0.693181;,
  0.827423;-0.693181;,
  0.827423;-0.700369;,
  0.838650;-0.700369;,
  0.827423;-0.707558;,
  0.838650;-0.707558;,
  0.807455;-0.707558;,
  0.818512;-0.707558;,
  0.807455;-0.700369;,
  0.818512;-0.700369;,
  0.827423;-0.685992;,
  0.827423;-0.714746;,
  0.807455;-0.685992;,
  0.811958;-0.683741;,
  0.807455;-0.714746;,
  0.615524;-0.521575;,
  0.977179;-0.008270;,
  0.811546;-0.690929;,
  0.807455;-0.693181;,
  0.811958;-0.690929;,
  0.615194;-0.925362;,
  0.811958;-0.691077;,
  0.024297;-0.522231;,
  0.575688;-0.007815;,
  0.024297;-0.522231;,
  0.978804;-0.500852;,
  0.615524;-0.521575;,
  0.573758;-0.500098;,
  0.807455;-0.700220;,
  0.026178;-0.924100;;
 }

 VertexDuplicationIndices {
  52;
  16;
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  0,
  0,
  0,
  1,
  1,
  2,
  2,
  2,
  3,
  3,
  4,
  4,
  5,
  5,
  6,
  6,
  7,
  7,
  8,
  8,
  8,
  9,
  9,
  9,
  10,
  10,
  11,
  11,
  12,
  12,
  13,
  13,
  14,
  14,
  15,
  15;
 }

 MeshMaterialList {
  1;
  28;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;

  Material CityPart1_initialShadingGroup {
   0.000000;0.000000;0.000000;1.000000;;
   40.000000;
   0.000000;0.000000;0.000000;;
   1.000000;1.000000;1.000000;;

   TextureFilename {
    "CityPart1.jpg";
   }
  }
 }

 XSkinMeshHeader {
  0;
  0;
  0;
 }
}