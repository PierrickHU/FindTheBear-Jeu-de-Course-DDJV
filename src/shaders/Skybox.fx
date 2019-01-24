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

   float2 remplissage;
   float2 centreTerrain;   // Le centre du terrain
}

Texture2D texture0;  // la texture
SamplerState SampleState;  // l'état de sampling
float rotationAngle = 0.40f;
float assombrissement = 0.15f;
float Epsilon = 1e-10;
int cauchemar = 0;

struct VS_Sortie
{
   float4 Pos : SV_Position;
   float3 Norm :    TEXCOORD0;
   float3 vDirLum : TEXCOORD1;
   float3 vDirCam : TEXCOORD2;
   float2 coordTex : TEXCOORD3;
};

float3 HUEtoRGB(in float H)
{
   float R = abs(H * 6 - 3) - 1;
   float G = 2 - abs(H * 6 - 2);
   float B = 2 - abs(H * 6 - 4);
   return saturate(float3(R, G, B));
}

float3 RGBtoHCV(in float3 RGB)
{
   // Based on work by Sam Hocevar and Emil Persson
   float4 P = (RGB.g < RGB.b) ? float4(RGB.bg, -1.0, 2.0 / 3.0) : float4(RGB.gb, 0.0, -1.0 / 3.0);
   float4 Q = (RGB.r < P.x) ? float4(P.xyw, RGB.r) : float4(RGB.r, P.yzx);
   float C = Q.x - min(Q.w, Q.y);
   float H = abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
   return float3(H, C, Q.x);
}

float3 HSLtoRGB(in float3 HSL)
{
   float3 RGB = HUEtoRGB(HSL.x);
   float C = (1 - abs(2 * HSL.z - 1)) * HSL.y;
   return (RGB - 0.5) * C + HSL.z;
}


float3 RGBtoHSL(in float3 RGB)
{
   float3 HCV = RGBtoHCV(RGB);
   float L = HCV.z - HCV.y * 0.5;
   float S = HCV.y / (1 - abs(L * 2 - 1) + Epsilon);
   return float3(HCV.x, S, L);
}

float3 rotationCouleur(in float angle, in float3 color) {
   float3 colorHSL = RGBtoHSL(color);
   colorHSL.x = fmod(colorHSL.x + angle, 1.0f);
   return HSLtoRGB(colorHSL);
}

float3 assombrirPixel(in float valeur, in float3 color) {
   float3 colorHSL = RGBtoHSL(color);
   colorHSL.z = max(colorHSL.z - valeur, 0.0f);
   return HSLtoRGB(colorHSL);
}


VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
   VS_Sortie sortie = (VS_Sortie)0;

   sortie.Pos = mul(Pos, matWorldViewProj);
   sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

   float3 PosWorld = mul(Pos, matWorld).xyz;

   sortie.vDirLum = posLumiere.xyz - PosWorld;
   sortie.vDirCam = 0;

   // Coordonnées d'application de texture
   sortie.coordTex = coordTex;

   return sortie;
}

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
   float3 couleur;

// Normaliser les paramètres
float3 N = normalize(vs.Norm);
float3 L = normalize(vs.vDirLum);
float3 V = normalize(vs.vDirCam);

// Valeur de la composante diffuse
float3 diff = saturate(dot(N, L));

// R = 2 * (N.L) * N – L
float3 R = normalize(2 * diff * N - L);

// Puissance de 4 - pour l'exemple
float S = pow(saturate(dot(R, V)), 12.0f);

// Échantillonner la couleur du pixel à partir de la texture
float3 couleurTexture = texture0.Sample(SampleState, vs.coordTex).rgb;

// I = A + D * N.L + (R.V)n
couleur = couleurTexture * vAEcl.rgb * vAMat.rgb;

couleur += S;
if (cauchemar) {

   couleur = assombrirPixel(assombrissement, couleur);
   couleur = rotationCouleur(rotationAngle , couleur);
   couleur.r = min(couleur.r*1.5f, 1.0f);

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
