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
   float2 centreTerrain;   // Le centre du terraind
}

struct ShadowMapVS_SORTIE
{
   float4 Pos : SV_POSITION;
   float3 Profondeur:	TEXCOORD0;
};

//-------------------------------------------------------------------
// Vertex Shader pour construire le shadow map
//-------------------------------------------------------------------
ShadowMapVS_SORTIE ShadowMapVS(float4 Pos : POSITION)
{
   ShadowMapVS_SORTIE Out = (ShadowMapVS_SORTIE)0;

   // Calcul des coordonn�es
   Out.Pos = mul(Pos, matWorldViewProjLight);  // WVP de la lumiere

   // Obtenir la profondeur et normaliser avec w
   Out.Profondeur.x = Out.Pos.z / Out.Pos.w;

   return Out;
}

RasterizerState rsCullFront
{
   CullMode = Front;
};

//-------------------------------------------------------------------
// Technique pour le shadow map
//-------------------------------------------------------------------

Texture2D textureEntree;    // la texture
Texture2D texture0;    // la texture
SamplerState SampleState;   // l'�tat de sampling

Texture2D ShadowTexture;	// La texture du shadow map
SamplerState ShadowMapSampler
{
   Filter = MIN_MAG_MIP_POINT;
   AddressU = Clamp;
   AddressV = Clamp;
};

struct VS_Sortie
{
   float4 Pos : SV_Position;
   float3 Norm :    TEXCOORD0;
   float3 vDirLum : TEXCOORD1;
   float3 vDirCam : TEXCOORD2;
   float2 coordTex : TEXCOORD3;
   float4 PosInMap : TEXCOORD4;
};

//-----------------------------------------------------
// Vertex Shader miniphong avec shadow map
//-----------------------------------------------------
VS_Sortie MiniPhongVS(float4 Pos : POSITION,
   float3 Normale : NORMAL, float2 coordTex : TEXCOORD0)
{
   VS_Sortie sortie = (VS_Sortie)0;

   sortie.Pos = mul(Pos, matWorldViewProj);
   sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

   float3 PosWorld = mul(Pos, matWorld).xyz;

   sortie.vDirLum = (bTunnel) ? posLumiere.xyz - PosWorld : dirLumiere.xyz;
   sortie.vDirCam = vCamera.xyz - PosWorld;

   // Coordonn�es d'application de texture
   sortie.coordTex = coordTex;

   // Valeurs pour shadow map
   // Coordonn�es
   sortie.PosInMap = mul(Pos, matWorldViewProjLight);

   return sortie;
}

#define DIMTEX_SHADOW 2048
#define NB_INTERPOL 3
#define NB_COLONNE_MAT_INTERPOL 1 + 2*NB_INTERPOL
#define TAILLE_MAT_INTERPOL NB_COLONNE_MAT_INTERPOL * NB_COLONNE_MAT_INTERPOL 

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

   // R = 2 * (N.L) * N - L
   float3 R = normalize(2 * diff * N - L);

   // Calcul de la sp�cularit� 
   float3 S = (isnan(pow(saturate(dot(R, V)), puissance))) ? 0 : pow(saturate(dot(R, V)), puissance);

   // ********* TEXTURE *********
   float3 couleurTexture = vDMat.rgb;

   if (bTex)
   {
      // �chantillonner la couleur du pixel � partir de la texture
      couleurTexture = texture0.Sample(SampleState, vs.coordTex).rgb;
   }
   else
   {
       couleurTexture = vDMat.rgb;
   }

   // ********* OMBRE *********
   float ValeurOmbre;

   float2 ShadowCoord = 0.5f * vs.PosInMap.xy / vs.PosInMap.w + float2(0.5f, 0.5f);
   bool estEntre0et1 = saturate(ShadowCoord.xy) == ShadowCoord.xy;
   if (estEntre0et1) {
      ShadowCoord.y = 1.0f - ShadowCoord.y;

      float2 PixelActuel = DIMTEX_SHADOW * ShadowCoord; // Pour une texture de 512 X 512

      // Valeur de l'interpolation lin�aire
      float2 lerps = frac(PixelActuel);

      // Lire les valeurs du tableau, avec les v�rifications de profondeur
      float Profondeur = vs.PosInMap.z / vs.PosInMap.w;
      

      float echelle = 1.0 / DIMTEX_SHADOW;

      float kernel[NB_COLONNE_MAT_INTERPOL][NB_COLONNE_MAT_INTERPOL];
      float2 coord[NB_COLONNE_MAT_INTERPOL][NB_COLONNE_MAT_INTERPOL];
      //ligne
      for (int i = 0; i < NB_COLONNE_MAT_INTERPOL; i++) {
         //colonne
         for (int j = 0; j < NB_COLONNE_MAT_INTERPOL; j++) {
            float distanceOrigineCol = j - (NB_COLONNE_MAT_INTERPOL - 1) / 2.f;
            float distanceOrigineLigne = i - (NB_COLONNE_MAT_INTERPOL - 1) / 2.f;

            coord[i][j] = ShadowCoord + float2(distanceOrigineCol*echelle, distanceOrigineLigne*echelle);
         }
      }

      //ligne
      for (int i = 0; i < NB_COLONNE_MAT_INTERPOL; i++) {
         //colonne
         for (int j = 0; j < NB_COLONNE_MAT_INTERPOL; j++) {
            float couleurShadowTexture = ShadowTexture.Sample(ShadowMapSampler, coord[i][j]).r;
            kernel[i][j] = (couleurShadowTexture < Profondeur) ? 0.5f : 1.0f;
         }
      }

      //On interpole
      float colInter[NB_COLONNE_MAT_INTERPOL][NB_COLONNE_MAT_INTERPOL];
      float ligneInter[NB_COLONNE_MAT_INTERPOL][NB_COLONNE_MAT_INTERPOL];

      //ligne
      for (int i = 0; i < NB_COLONNE_MAT_INTERPOL; i++) {
         //colonne
         for (int j = 0; j < NB_COLONNE_MAT_INTERPOL - 1; j++) {
            colInter[i][j] = lerp(kernel[i][j], kernel[i][j + 1], lerps.x);
         }
      }

      //ligne
      for (int i = 0; i < NB_COLONNE_MAT_INTERPOL - 1; i++) {
         //colonne
         for (int j = 0; j < NB_COLONNE_MAT_INTERPOL - 1; j++) {
            ligneInter[i][j] = lerp(colInter[i][j], colInter[i + 1][j], lerps.y);
         }
      }

      
      // On fait la moyenne
      float nbVal = 0.0f;
      //ligne
      for (int i = 0; i < NB_COLONNE_MAT_INTERPOL - 1; i++) {
         //colonne
         for (int j = 0; j < NB_COLONNE_MAT_INTERPOL - 1; j++) {
            ValeurOmbre += ligneInter[i][j];
            nbVal++;
         }
      }
      ValeurOmbre /= (nbVal == 0.0f) ? 1.0f : nbVal;
   }
   else {
      ValeurOmbre = 1.0;
   }

   // ********* COULEUR FINALE *********
   // I = A + D * N.L + (R.V)n
   couleur = couleurTexture * vAEcl.rgb * vAMat.rgb
           + couleurTexture * vDEcl.rgb * vDMat.rgb * diff
           + vSEcl.rgb * vSMat.rgb * S;

   couleur *= ValeurOmbre; // on ajuste avec l'ombre

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

   return float4(couleur, 0.4f); // Le param�tre alpha n'est utilis� que si ActiverMelangeAlpha est utilis�. Du coup je me permet de le laisser ici ! :)
}

technique11 ShadowMap
{
   pass p0
   {
      VertexShader = compile vs_5_0 ShadowMapVS();
      SetRasterizerState(rsCullFront);

      SetPixelShader(NULL);
      SetGeometryShader(NULL);
   }
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
