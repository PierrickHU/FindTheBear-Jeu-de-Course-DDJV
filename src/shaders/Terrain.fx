cbuffer param
{
   float4x4 matWorldViewProj;   // la matrice totale 
   float4x4 matWorldViewProjLight; // Matrice WVP pour lumière
   float4x4 matWorld;		// matrice de transformation dans le monde 
   float4 posLumiere; 		// la position de la source d'éclairage (Point)
   float4 dirLumiere; 		// la direction de la source d'éclairage (lumiere directionnelle)
   float4 vCamera; 			// la position de la caméra
   float4 vAEcl; 			// la valeur ambiante de l'éclairage
   float4 vAMat; 			// la valeur ambiante du matériau
   float4 vDEcl; 			// la valeur diffuse de l'éclairage 
   float4 vDMat; 			// la valeur diffuse du matériau 
   float4 vSEcl; 			// la valeur spéculaire de l'éclairage 
   float4 vSMat; 			// la valeur spéculaire du matériau 
   float puissance; 		// la puissance de spécularité
   int bTex;		        // Booléen pour la présence de texture
   int bTunnel;
   float2 centreTerrain;   // Le centre du terrain
}

struct VS_Sortie
{
   float Height : height;
   float DistanceToCenter : DISTANCE_TO_CENTER;
   float4 Pos : SV_Position;
   float3 Norm :    TEXCOORD0;
   float3 vDirLum : TEXCOORD1;
   float3 vDirCam : TEXCOORD2;
   float2 coordTex : TEXCOORD3;
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
   VS_Sortie sortie = (VS_Sortie)0;

   sortie.Height = Pos.y;
   sortie.DistanceToCenter = sqrt((Pos.x - centreTerrain.x)*(Pos.x - centreTerrain.x) + (Pos.z - centreTerrain.y)*(Pos.z - centreTerrain.y));
   sortie.Pos = mul(Pos, matWorldViewProj);
   sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

   float3 PosWorld = mul(Pos, matWorld).xyz;

   //sortie.vDirLum = posLumiere.xyz - PosWorld;
   sortie.vDirLum = dirLumiere.xyz;
   sortie.vDirCam = vCamera.xyz - PosWorld;

   // Coordonnées d'application de texture
   sortie.coordTex = coordTex;

   return sortie;
}

Texture2D texture0;  // la texture
Texture2D texture1;  // la texture
Texture2D texture2;  // la ... texture ! :D
SamplerState SampleState;  // l'état de sampling

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
    float3 couleur;

   // ********* LUMIERE (effetPhong) *********
   // Normaliser les paramètres
   float3 N = normalize(vs.Norm);
   float3 L = normalize(vs.vDirLum);
   float3 V = normalize(vs.vDirCam);

   // Valeur de la composante diffuse
   float3 diff = saturate(dot(N, L));

   // R = 2 * (N.L) * N – L
   float3 R = normalize(2 * diff * N - L);

   // Puissance de 4 - pour l'exemple
   float S = pow(saturate(dot(R, V)), puissance);

   // ********* TEXTURE *********
   // Échantillonner la couleur du pixel à partir de la texture
   float seuilHaut = 50.f;
   float seuilBas = 30.f;
   float3 couleurTexture;
   // On calcul sa couleur en fonction de la hauteur
   if (vs.Height < seuilBas) {
       couleurTexture = texture0.Sample(SampleState, vs.coordTex).rgb;
   }
   else if (vs.Height > seuilHaut) {
       couleurTexture = texture1.Sample(SampleState, vs.coordTex).rgb;
   }
   else {
       float dx = (vs.Height - seuilBas) / (seuilHaut - seuilBas);
       couleurTexture = texture0.Sample(SampleState, vs.coordTex).rgb * (1.f - dx) + texture1.Sample(SampleState, vs.coordTex).rgb * dx;
   }

   // Puis on ajoute la distance au chateau
   float seuilDistanceChateauCourte = 200.f;
   float seuilDistanceChateauLongue = 300.f;
   if (vs.DistanceToCenter < seuilDistanceChateauCourte) {
       couleurTexture = texture2.Sample(SampleState, vs.coordTex).rgb;
   }
   else if (vs.DistanceToCenter < seuilDistanceChateauLongue) {
       float dx = (vs.DistanceToCenter - seuilDistanceChateauCourte) / (seuilDistanceChateauLongue - seuilDistanceChateauCourte);
       couleurTexture = couleurTexture * dx + texture2.Sample(SampleState, vs.coordTex).rgb * (1.0f - dx);
   }
   else {}

   // ********* COULEUR FINALE *********
   // I = A + D * N.L + (R.V)n
   couleur = couleurTexture * vAEcl.rgb * vAMat.rgb +
              couleurTexture * vDEcl.rgb * vDMat.rgb * diff;

   couleur += vSEcl.rgb * vSMat.rgb * S;

   // ********* TUNNEL **********
   if (bTunnel > 0) {
      float ombreTunnel = 0.f;
      float3 lightDirection = dirLumiere;
      float theta = 10.0f;
      float phi = 30.0f;
      float angleMax = 45.f;
      float angleMin = 20.f;
      float cosMax = cos(radians(angleMax));
      float cosMin = cos(radians(angleMin));

      float cos = dot(normalize(lightDirection), normalize(vs.vDirLum));
      
      // en dehors
      if (cos < cosMax) {
         ombreTunnel = 0.0f;
      }
      ////en plein coeur
      else if (cos > cosMin) {
         ombreTunnel = 1.0f;
      }
      else {
         //   ombreTunnel = \*smoothstep(cosMax, cosMin, cos)*\;
      }

      couleur *= ombreTunnel;
   }

   return float4(couleur, 1.0f);
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
