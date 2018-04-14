cbuffer cbTransform : register(b0) {
	matrix mTransform;
	matrix mViewProj;
};

cbuffer ConstantBuffer : register(b1) {
	float4 color;
	float4 params;// ->
								/*  ->  UINT normalTabIndex;
								UINT curframe;
								UINT oldframe;
								FLOAT backlerp;*/
};

Texture2D<uint4> vertexMap : register(t1);
Texture2D<float3> vertScaleTranslateMap : register(t2);
Texture2D<float> normalTable : register(t3);
Texture2D<float> normals : register(t4);

Texture2D<float4> image : register(t0);
SamplerState imageSampler : register(s0);

struct VSInput {
	uint pos : POSITION0;
	float2 Tex : TEXCOORD0;
};

struct VSOutput {
	float4 posCS : SV_POSITION;
	//float2 Tex : TEXCOORD0;
	float4 Color : COLOR;
};

VSOutput VS(in VSInput input) {
	VSOutput output;
	uint normalTabIndex = (uint)params.x;
	uint curframe = (uint)params.y;
	uint oldframe = (uint)params.z;
	float backlerp = params.w;
	float4 oldpos = vertexMap[uint2(input.pos, oldframe)];
	float3 oldscale = vertScaleTranslateMap[uint2(0, oldframe)];
	float3 oldtranslate = vertScaleTranslateMap[uint2(1, oldframe)];
	float4 curpos = vertexMap[uint2(input.pos, curframe)];
	float3 curscale = vertScaleTranslateMap[uint2(0, curframe)];
	float3 curtranslate = vertScaleTranslateMap[uint2(1, curframe)];

	oldpos.xyz = oldpos.xyz * oldscale.xyz + oldtranslate.xyz;
	curpos.xyz = curpos.xyz * curscale.xyz + curtranslate.xyz;

	float3 pos;
	//float frontlerp = 1.0f - backlerp;
	pos.xyz = /*move.xyz * backlerp + */curpos.xyz/* * backlerp + curpos.xyz * frontlerp*/;
	output.posCS = mul(float4(pos, 1.0f), mTransform);

	/*
	output.Tex = input.Tex;*/
	float normal = normalTable[uint2(normalTabIndex, (uint)curpos.w)]/* * backlerp + normalTable[uint2(normalTabIndex, (uint)curpos.w)] * frontlerp*/;
	output.Color.xyz = color.xyz * normal;
	output.Color.w = color.w;

	return output;
}

float4 PS(in VSOutput input) : SV_Target {

	return input.Color;
}
