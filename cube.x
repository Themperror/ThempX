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
 24;
 -0.583067;0.000000;0.539810;,
 0.539810;1.368936;0.539810;,
 0.539810;0.000000;0.539810;,
 -0.583067;1.368936;0.539810;,
 0.539810;1.368936;-0.628478;,
 -0.583067;1.368936;-0.628478;,
 0.539810;0.000000;-0.628478;,
 -0.583067;0.000000;-0.628478;,
 -0.583067;0.000000;0.539810;,
 -0.583067;0.000000;0.539810;,
 0.539810;1.368936;0.539810;,
 0.539810;1.368936;0.539810;,
 0.539810;0.000000;0.539810;,
 0.539810;0.000000;0.539810;,
 -0.583067;1.368936;0.539810;,
 -0.583067;1.368936;0.539810;,
 0.539810;1.368936;-0.628478;,
 0.539810;1.368936;-0.628478;,
 -0.583067;1.368936;-0.628478;,
 -0.583067;1.368936;-0.628478;,
 0.539810;0.000000;-0.628478;,
 0.539810;0.000000;-0.628478;,
 -0.583067;0.000000;-0.628478;,
 -0.583067;0.000000;-0.628478;;
 12;
 3;9,13,11;,
 3;9,11,15;,
 3;14,10,17;,
 3;14,17,19;,
 3;18,16,21;,
 3;18,21,23;,
 3;22,20,12;,
 3;22,12,8;,
 3;2,6,4;,
 3;2,4,1;,
 3;7,0,3;,
 3;7,3,5;;

 MeshNormals {
  24;
  -1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;0.000000;-1.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;0.000000;-1.000000;;
  12;
  3;9,13,11;,
  3;9,11,15;,
  3;14,10,17;,
  3;14,17,19;,
  3;18,16,21;,
  3;18,21,23;,
  3;22,20,12;,
  3;22,12,8;,
  3;2,6,4;,
  3;2,4,1;,
  3;7,0,3;,
  3;7,3,5;;
 }

 MeshTextureCoords {
  24;
  0.375000;-0.000000;,
  0.625000;-0.250000;,
  0.625000;-0.000000;,
  0.375000;-0.250000;,
  0.875000;-0.250000;,
  0.125000;-0.250000;,
  0.875000;-0.000000;,
  0.125000;-0.000000;,
  0.375000;-1.000000;,
  0.375000;-0.000000;,
  0.625000;-0.250000;,
  0.625000;-0.250000;,
  0.625000;-1.000000;,
  0.625000;-0.000000;,
  0.375000;-0.250000;,
  0.375000;-0.250000;,
  0.625000;-0.500000;,
  0.625000;-0.500000;,
  0.375000;-0.500000;,
  0.375000;-0.500000;,
  0.625000;-0.750000;,
  0.625000;-0.750000;,
  0.375000;-0.750000;,
  0.375000;-0.750000;;
 }

 VertexDuplicationIndices {
  24;
  8;
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  0,
  0,
  1,
  1,
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
  7;
 }

 MeshMaterialList {
  1;
  12;
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

  Material initialShadingGroup {
   0.000000;0.000000;0.000000;1.000000;;
   40.000000;
   0.000000;0.000000;0.000000;;
   1.000000;1.000000;1.000000;;

   TextureFilename {
    "texture.jpg";
   }
  }
 }

 XSkinMeshHeader {
  0;
  0;
  0;
 }
}