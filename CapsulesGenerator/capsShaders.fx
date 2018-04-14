cbuffer cbTransform : register(b0) {
	matrix mTransform;
	matrix mViewProj;
};

cbuffer CapsConstantBuffer : register(b1) {
	matrix mWorld;
	float4 capsColor;
};

Texture2D<float2> zBuffer : register(t0);

struct VSCapsInput {
	float3 pos : POSITION0;
};

struct VSOutput {
	float4 posCS : SV_POSITION;
	float4 Color : COLOR;
};

VSOutput VS(in VSCapsInput input) {
	VSOutput output;

	//matrix Transform = transpose(mul(mWorld, mViewProj));
	//output.posCS = mul(float4(input.pos, 1.0f), Transform);

	output.posCS = mul(float4(input.pos, 1.0f), mWorld);
	output.posCS = mul(output.posCS, mViewProj);

	output.Color = capsColor;

	return output;
}

float4 PS(in VSOutput input) : SV_Target{

	//float depth = zBuffer.Load(int3(input.posCS.xy, 0)).r;

  //if(depth > input.posCS.z)
		return input.Color;
	//return float4(0.0f, 0.0f, 0.0f, 0.0f);
}
