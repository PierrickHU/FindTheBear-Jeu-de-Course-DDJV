struct VS_Sortie
{
	float4 Pos : SV_POSITION;
	float2 CoordTex : TEXCOORD0;
};

Texture2D texture0;    // la bonne texture
SamplerState SampleState;   // l'etat de sampling

float distance;
float puissanceAttenuation;
float puissanceEclaircissement;

//-------------------------------------------------------
// Vertex Shader ?Nul?
//-------------------------------------------------------
VS_Sortie NulVS(float4 Pos : POSITION, float2 CoordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = Pos;
	sortie.CoordTex = CoordTex;

	return sortie;
}

//-----------------------------------------------------
// Pixel Shader ?Nul?
//-----------------------------------------------------
float4 NulPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;

	couleur = texture0.Sample(SampleState, vs.CoordTex);

	return couleur;
}

float4 RadialBlurPS(VS_Sortie vs) : SV_Target
{
	float4 couleur; // couleur final du pixel
	float4 ct; // couleur de la texture
	float2 tc = vs.CoordTex; // coordonne dans la texture
	float d, dx, dy;

	couleur = 0;
	float x = tc.x * 2 - 1.0; // on passe de [0,1] a [-1,1]
	float y = tc.y * 2 - 1.0;
	dx = sqrt(x*x);  // Distance du centre
	dy = sqrt(y*y);  // Distance du centre

	dx = x * (distance*dx);  	// Le degrade (blur) est en fonction de la
	dy = y * (distance*dy);	 	// distance du centre et de la variable distance.

    uint nbIteration = 100;
	x = x - (dx * nbIteration);   // Vous pouvez jouer avec le nombre d'iterations
	y = y - (dy * nbIteration);

	tc.x = (x + 1.0) / 2.0;
	tc.y = (y + 1.0) / 2.0;

	for (int i = 0; i < nbIteration; i++)   // Vous pouvez jouer avec le nombre d'iterations
	{
	   ct = texture0.Sample(SampleState, tc);
	   couleur = couleur * 0.6 + ct * 0.4; // Vous pouvez jouer avec les %

	   x = x + dx;
	   y = y + dy;

	   tc.x = (x + 1.0) / 2.0;
	   tc.y = (y + 1.0) / 2.0;
	}

    // Niveaux de gris
    float luminance = 0.2126f * couleur.r + 0.7152f * couleur.g + 0.0722f * couleur.b;

    // Puis on rends de plus en plus sombre sur les côtés
    if (puissanceAttenuation > 0.f) {
        d = sqrt(x * x + y * y);
        float coefAttenuation = pow((1.f - d), puissanceAttenuation);
        float coefEclaircissement = puissanceEclaircissement;

        float4 blackColor;
        blackColor.r = couleur.r * coefAttenuation;
        blackColor.g = couleur.g * coefAttenuation;
        blackColor.b = couleur.b * coefAttenuation;

        couleur.r = blackColor.r * coefEclaircissement;
        couleur.g = blackColor.g * coefEclaircissement;
        couleur.b = blackColor.b * coefEclaircissement;
    }

	return couleur;
}

float4 GrayScalePS(VS_Sortie vs) : SV_Target
{
	float4 couleur; // couleur final du pixel
	float2 tc = vs.CoordTex; // coordonne dans la texture
    couleur = texture0.Sample(SampleState, tc);

    float luminance = 0.2126f * couleur.r + 0.7152f * couleur.g + 0.0722f * couleur.b;
    couleur.r = luminance;
    couleur.g = luminance;
    couleur.b = luminance;

	return couleur;
}

technique11 Nul
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 NulPS();
		SetGeometryShader(NULL);
	}
}

technique11 RadialBlur
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 RadialBlurPS();
		SetGeometryShader(NULL);
	}
}

technique11 GrayScale
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 GrayScalePS();
		SetGeometryShader(NULL);
	}
}
