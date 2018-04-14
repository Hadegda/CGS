cbuffer cbTransform : register(b0) {
	matrix mTransform;
	matrix mViewProj;
};


struct VSGAInput {
	float3 pos : POSITION0;
	uint claster : CLASTER0;
};

struct VSOutput {
	float4 posCS : SV_POSITION;
	float4 Color : COLOR;
};

VSOutput VS(in VSGAInput input) {
	VSOutput output;

	//matrix Transform = transpose(mul(mWorld, mViewProj));
	//output.posCS = mul(float4(input.pos, 1.0f), Transform);

	output.posCS = mul(float4(input.pos.xyz, 1.0f), mTransform);

	if (input.claster == 100)
		output.Color = float4(1.0f, 0.5f, 0.0f, 0.2f);

	if(input.claster == 0 || input.claster == 3)
		output.Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	if (input.claster == 1 || input.claster == 11)
		output.Color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	if (input.claster == 2 || input.claster == 10)
		output.Color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	if (input.claster == 4 || input.claster == 8)
		output.Color = float4(1.0f, 1.0f, 0.0f, 1.0f);
	if (input.claster == 5 || input.claster == 7)
		output.Color = float4(1.0f, 0.0f, 1.0f, 1.0f);
	if (input.claster == 6 || input.claster == 13)
		output.Color = float4(0.0f, 1.0f, 1.0f, 1.0f);
	if (input.claster == 9 || input.claster == 12)
		output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return output;
}

float4 PS(in VSOutput input) : SV_Target{

	return input.Color;
}