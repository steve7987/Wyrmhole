//vertex shader

//globals

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float textureOffset;
};

cbuffer TubeLayoutBuffer
	{
		float3 p1;  //position of the four points of the tube
		float s1;  //distances of each of the sections
		float3 p2;
		float s2;
		float3 p3;
		float s3;
		float3 p4;
		float maxAngle;  //max angle for turning circle
		float3 Center;  //center of the turning circle
		float radius;
		float3 v1;  //unit vectors for parameterizing the circle
		float pad1;
		float3 v2;
		float pad2;
			
	};

//structs

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float distance : POSITION1;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

//Vertex shader

PixelInputType TubeVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
	//adjust input position based on distance
	float3 oldCenter;
	oldCenter.x = 1;
	oldCenter.y = 0;
	oldCenter.z = 0;
	oldCenter = oldCenter * input.distance;
	if (input.distance <= s1){
		float t = input.distance / s1;
		input.position.x = input.position.x - oldCenter.x + (1 - t)*p1.x + t*p2.x;
		input.position.y = input.position.y - oldCenter.y + (1 - t)*p1.y + t*p2.y;
		input.position.z = input.position.z - oldCenter.z + (1 - t)*p1.z + t*p2.z;
	}
	else if (input.distance <= s1 + s2){
		float t = (input.distance - s1) / s2 * maxAngle;
		input.position.x = input.position.x - oldCenter.x + Center.x + radius*(v1.x*cos(t) + v2.x*sin(t));
		input.position.y = input.position.y - oldCenter.y + Center.y + radius*(v1.y*cos(t) + v2.y*sin(t));
		input.position.z = input.position.z - oldCenter.z + Center.z + radius*(v1.z*cos(t) + v2.z*sin(t));
	}
	else {
		float t = (input.distance - s1 - s2) / s3;
		input.position.x = input.position.x - oldCenter.x + (1 - t)*p3.x + t*p4.x;
		input.position.y = input.position.y - oldCenter.y + (1 - t)*p3.y + t*p4.y;
		input.position.z = input.position.z - oldCenter.z + (1 - t)*p3.z + t*p4.z;
	}


    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
	output.tex.x += textureOffset;
    
    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	//calculate the world position
	worldPosition = mul(input.position, worldMatrix);

	//get view direction
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

    return output;
}