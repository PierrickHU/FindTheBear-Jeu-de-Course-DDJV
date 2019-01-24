cbuffer param
{
   float4x4 matWorldViewProj;   // la matrice totale 
   float4x4 matWorldViewProjLight; // Matrice WVP pour lumi�re
   float4x4 matWorld;		// matrice de transformation dans le monde 
   float4 posLumiere; 		// la position de la source d'�clairage (Point)
   float4 dirLumiere; 		// la direction de la source d'�clairage (lumiere directionnelle)
   float4 vCamera; 			// la position de la cam�ra
   float4 vAEcl; 			// la valeur ambiante de l'�clairage
   float4 vAMat; 			// la valeur ambiante du mat�riau
   float4 vDEcl; 			// la valeur diffuse de l'�clairage 
   float4 vDMat; 			// la valeur diffuse du mat�riau 
   float4 vSEcl; 			// la valeur sp�culaire de l'�clairage 
   float4 vSMat; 			// la valeur sp�culaire du mat�riau 
   float puissance; 		// la puissance de sp�cularit�
   int bTex;		        // Bool�en pour la pr�sence de texture
   int bTunnel;
   float2 centreTerrain;   // Le centre du terrain
}

struct VS_Sortie
{
   float4 Pos : SV_Position;
   float3 Norm :    TEXCOORD0;
   float3 vDirLum : TEXCOORD1;
   float3 vDirCam : TEXCOORD2;
   float2 coordTex : TEXCOORD3;
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
   VS_Sortie sortie = (VS_Sortie)0;

   sortie.Pos = mul(Pos, matWorldViewProj);
   sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

   float3 PosWorld = mul(Pos, matWorld).xyz;

   //sortie.vDirLum = posLumiere.xyz - PosWorld;
   sortie.vDirLum = (bTunnel) ? posLumiere.xyz - PosWorld : dirLumiere.xyz;
   sortie.vDirCam = vCamera.xyz - PosWorld;

   // Coordonn�es d'application de texture
   sortie.coordTex = coordTex;

   return sortie;
}

Texture2D textureEntree;  // la texture
Texture2D texture0;  // la texture
SamplerState SampleState;  // l'�tat de sampling

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
    float3 couleur;

   // ********* LUMIERE (effetPhong) *********
   // Normaliser les param�tres
   float3 N = normalize(vs.Norm);
   float3 L = normalize(vs.vDirLum);
   float3 V = normalize(vs.vDirCam);

   // Valeur de la composante diffuse
   float3 diff = saturate(dot(N, L));

   // R = 2 * (N.L) * N � L
   float3 R = normalize(2 * diff * N - L);

   // Puissance de 4 - pour l'exemple
   float S = pow(saturate(dot(R, V)), 12.0f);

   // ********* TEXTURE *********
   if (bTex) {
      // �chantillonner la couleur du pixel � partir de la texture
      float3 couleurTexture = texture0.Sample(SampleState, vs.coordTex).rgb;

      // I = A + D * N.L + (R.V)n
      couleur = couleurTexture * vAEcl.rgb * vAMat.rgb +
      couleurTexture * vDEcl.rgb * vDMat.rgb * diff;
   }
   else {
       couleur = vAEcl.rgb * vAMat.rgb +
           vDEcl.rgb * vDMat.rgb * diff;
   }

   // ********* COULEUR FINALE *********
   couleur += vSEcl.rgb * vSMat.rgb * S;

   // ********* TUNNEL **********
   if (bTunnel > 0) {
      float ombreTunnel = 0.5f;
      float3 lightDirection = dirLumiere;
      float angleMax = 45.f;
      float angleMin = 20.f;
      float cosMax = cos(radians(angleMax));
      float cosMin = cos(radians(angleMin));

      float cos = dot(normalize(lightDirection), normalize(vs.vDirLum));
      
      // en dehors
      if (cos < cosMax) {
         ombreTunnel = 0.0f;
      }
      //en plein coeur
      else if (cos > cosMin) {
         ombreTunnel = 1.0f;
      }
      else {
         ombreTunnel = smoothstep(cosMax, cosMin, cos);
      }

      couleur *= ombreTunnel;
   }

   return float4(couleur, texture0.Sample(SampleState, vs.coordTex).a);
}

technique11 MiniPhong
{
   pass pass0
   {
      SetVertexShader(CompileShader(vs_5_0, MiniPhongVS()));
      SetPixelShader(CompileShader(ps_5_0, MiniPhongPS()));
      SetGeometryShader(NULL);
   }
}
