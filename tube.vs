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
		float3 offset = radius*(v1*cos(t) + v2*sin(t));
		
		//compute rotation matrix
		float3 u = normalize(cross(v2, v1));  //rotation axis
		float3x3 rotationMatrix;
		rotationMatrix._11 = cos(t) + u.x*u.x*(1-cos(t));
		rotationMatrix._12 = u.x*u.y*(1-cos(t)) - u.z*sin(t);
		rotationMatrix._13 = u.x*u.z*(1-cos(t)) + u.y*sin(t);

		rotationMatrix._21 = u.y*u.x*(1-cos(t)) + u.z*sin(t);
		rotationMatrix._22 = cos(t) + u.y*u.y*(1-cos(t));
		rotationMatrix._23 = u.y*u.z*(1-cos(t)) - u.x*sin(t);

		rotationMatrix._31 = u.z*u.x*(1-cos(t)) - u.y*sin(t);
		rotationMatrix._32 = u.z*u.y*(1-cos(t)) + u.x*sin(t);
		rotationMatrix._33 = cos(t) + u.z*u.z*(1-cos(t));

		//rotated ring of the tube
		float3 rotated;
		rotated.x = input.position.x - oldCenter.x;
		rotated.y = input.position.y - oldCenter.y;
		rotated.z = input.position.z - oldCenter.z;
		rotated = mul(rotated, rotationMatrix);
		
		//compute final position
		input.position.x = rotated.x + Center.x + offset.x; 
		input.position.y = rotated.y + Center.y + offset.y; 
		input.position.z = rotated.z + Center.z + offset.z; 

	}
	else {
		float t = maxAngle;
		//compute rotation matrix
		float3 u = normalize(cross(v2, v1));  //rotation axis
		float3x3 rotationMatrix;
		rotationMatrix._11 = cos(t) + u.x*u.x*(1-cos(t));
		rotationMatrix._12 = u.x*u.y*(1-cos(t)) - u.z*sin(t);
		rotationMatrix._13 = u.x*u.z*(1-cos(t)) + u.y*sin(t);

		rotationMatrix._21 = u.y*u.x*(1-cos(t)) + u.z*sin(t);
		rotationMatrix._22 = cos(t) + u.y*u.y*(1-cos(t));
		rotationMatrix._23 = u.y*u.z*(1-cos(t)) - u.x*sin(t);

		rotationMatrix._31 = u.z*u.x*(1-cos(t)) - u.y*sin(t);
		rotationMatrix._32 = u.z*u.y*(1-cos(t)) + u.x*sin(t);
		rotationMatrix._33 = cos(t) + u.z*u.z*(1-cos(t));

		//rotated ring of the tube
		float3 rotated;
		rotated.x = input.position.x - oldCenter.x;
		rotated.y = input.position.y - oldCenter.y;
		rotated.z = input.position.z - oldCenter.z;
		rotated = mul(rotated, rotationMatrix);

		//compute final position
		float s = (input.distance - s1 - s2) / s3;
		input.position.x = rotated.x + (1 - s)*p3.x + s*p4.x;
		input.position.y = rotated.y + (1 - s)*p3.y + s*p4.y;
		input.position.z = rotated.z + (1 - s)*p3.z + s*p4.z;
	}


    // Calculate the position of the vertex against the world, view, and projection matrices.
    //output.position = mul(input.position, worldMatrix);  //for the moment world matrix does nothing
	output.position = input.position;
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