#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h" //ShaderCore module
#include "UniformBuffer.h" // RenderCore module
#include "RHICommandList.h" // RHI module


// Here we declare the struct as a variable name, and how our structure looks like
// The struct has to be implemented in .cpp using IMPLEMENT_UNIFORM_BUFFER_STRUCT
//This should contain variables that never, or rarely change
BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(SlowUniformBuffer, )//A7
SHADER_PARAMETER(float, y)//A2B
SHADER_PARAMETER(float, z)//A2A
END_GLOBAL_SHADER_PARAMETER_STRUCT()

//This buffer is for variables that change very often (each frame for example)
//BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FastUniformBuffer, )//A7
//SHADER_PARAMETER(float, y)//A2B
//SHADER_PARAMETER(float, z)//A2A
//END_GLOBAL_SHADER_PARAMETER_STRUCT()


/*****************************************************************************/
/* This class is what encapsulates the shader in the engine.                 */
/* It is the main bridge between the HLSL (.usf file) and the engine itself. */
/*****************************************************************************/
class /*COMPUTESHADERTEST419_API*/ FGlobalComputeShader_Interface : public FGlobalShader {
public:
	/* Essential functions */

	//Here we declare that we are USING a shader
	DECLARE_SHADER_TYPE(FGlobalComputeShader_Interface, Global, );
	//Not exporting to an external module
	//DECLARE_EXPORTED_SHADER_TYPE(FGlobalComputeShader_Interface, Global, );
	//DECLARE_EXPORTED_SHADER_TYPE(FGlobalComputeShader_Interface, Global, SHADERCONTROLLER_API);

	FGlobalComputeShader_Interface() {}
	explicit FGlobalComputeShader_Interface(const ShaderMetaType::CompiledShaderInitializerType& initializer);


	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& PermutationParams) {
		// Useful when adding a permutation of a particular shader
		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& PermutationParams, FShaderCompilerEnvironment& OutEnvironment) {
		FGlobalShader::ModifyCompilationEnvironment(PermutationParams, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
		//OutEnvironment.CompilerFlags.Add(CFLAG_OnChip);
		OutEnvironment.CompilerFlags.Add(CFLAG_PreferFlowControl);
		OutEnvironment.CompilerFlags.Add(CFLAG_Debug);
		OutEnvironment.CompilerFlags.Add(CFLAG_KeepDebugInfo);

		// Add your own defines for the shader code
		//OutEnvironment.SetDefine(TEXT("MY_DEFINE"), 1);
		//OutEnvironment.SetDefine(TEXT("NAME"), TEXT("Test"));
	}


	static bool ShouldCache(EShaderPlatform platform) {
		// Could skip compiling if the platform does not support DirectX Shader Model 5, for example, like the following.
		return IsFeatureLevelSupported(platform, ERHIFeatureLevel::SM5);
	}

	virtual bool Serialize(FArchive& Ar) override;




	/* Custom functions */

	void SetShaderParameters(FRHICommandList& RHICmdList, float dT, int gridX, int gridY, int gridZ, int gridSizeI, int gridSizeJ,
		float simulationTime, int prevGC, int currGC, int nextGC);
	void SetUniformBuffers(FRHICommandList& RHICmdList, const float offset_y, const float offset_z);

	// FShaderResourceViewRHIRef for StructuredBuffer, FUnorderedAccessViewRHIParamRef for RWStructuredBuffer.
	void SetShaderResourceParameters(FRHICommandList& RHICmdList 
		//FShaderResourceViewRHIRef FStruct_Cell_gridSizeK_CPU_ResourceParameter_SRV,
		//FShaderResourceViewRHIRef FStruct_GroundGridContainer_ground_CPU_ResourceParameter_SRV,
		//FShaderResourceViewRHIRef FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter_SRV,
		//FShaderResourceViewRHIRef FStruct_AirGridContainer_gridInit_CPU_ResourceParameter_SRV
	);


	void SetOutput(FRHICommandList& RHICmdList, 
		FRHIUnorderedAccessView* A_output,
		FRHIUnorderedAccessView* B_output,
		FRHIUnorderedAccessView* C_output 
	);
	void ClearParameters(FRHICommandList& RHICmdList); // for StructuredBuffer.
	void ClearOutput(FRHICommandList& RHICmdList); // for RWStructuredBuffer.

private:

	int buffer_index_rotator = 1;

	//These are individual variables to be used inside of the shader

	//FShaderParameter offset_x_; // float test_offset_x;
	FShaderParameter dTParameter;
	FShaderParameter gridXParameter;
	FShaderParameter gridYParameter;
	FShaderParameter gridZParameter;
	FShaderParameter gridSizeIParameter;
	FShaderParameter gridSizeJParameter;
	FShaderParameter simulationTimeParameter;
	FShaderParameter prevGCParameter;
	FShaderParameter currGCParameter;
	FShaderParameter nextGCParameter;

	//FShaderResourceParameter input_position_; // StructuredBuffer<float3> test_input_position;
	//FShaderResourceParameter input_scalar_; // StructuredBuffer<float> test_input_scalar;
	FShaderResourceParameter FStruct_Cell_gridSizeK_CPU_ResourceParameter; //single stack of values
	FShaderResourceParameter FStruct_GroundGridContainer_ground_CPU_ResourceParameter; //100
	FShaderResourceParameter FStruct_AirGridContainer_gridRslow_CPU_ResourceParameter; //x5600
	FShaderResourceParameter FStruct_AirGridContainer_gridInit_CPU_ResourceParameter; //x5600



	//THIS IS OUTPUT
	FShaderResourceParameter A_output_; // RWStructuredBuffer<float3> test_outputA;
	FShaderResourceParameter B_output_; // RWStructuredBuffer<float3> test_outputB;
	FShaderResourceParameter C_output_; // RWStructuredBuffer<float3> test_outputC;






	//class FSplatVolumeCS : public FGlobalShader
	//{
	//	DECLARE_GLOBAL_SHADER(FSplatVolumeCS);
	//	SHADER_USE_PARAMETER_STRUCT(FSplatVolumeCS, FGlobalShader);

	//	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	//	{
	//		return true;
	//	}

	//	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	//	{
	//		OutEnvironment.CompilerFlags.Add(CFLAG_ForceDXC);
	//	}

	//	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
	//		SHADER_PARAMETER(FIntVector, VolumeSize)
	//		SHADER_PARAMETER(int32, BrickSize)
	//		SHADER_PARAMETER(int32, bIsHighestMip)
	//		SHADER_PARAMETER_UAV(RWTexture3D<uint>, VoxelizeVolume)
	//		SHADER_PARAMETER_UAV(RWTexture3D<uint4>, IndirectionTexture)
	//		SHADER_PARAMETER_UAV(RWBuffer<int>, BrickAllocatorParameters)
	//		END_SHADER_PARAMETER_STRUCT()
	//};









	//FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	//TShaderMapRef<FSplatVolumeCS> ComputeShader(GlobalShaderMap);

	//FSplatVolumeCS::FParameters Parameters;
	//Parameters.VolumeSize = IndirectionTextureDimensions;
	//Parameters.BrickSize = 1 << (MipLevel * BrickSizeLog2);
	//Parameters.bIsHighestMip = MipLevel == VoxelizationVolumeMips.Num() - 1;
	//Parameters.VoxelizeVolume = VoxelizationVolumeMips[MipLevel]->GetRenderTargetItem().UAV;
	//Parameters.IndirectionTexture = IndirectionTexture->GetRenderTargetItem().UAV;
	//Parameters.BrickAllocatorParameters = BrickAllocatorParameters.UAV;

	//FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, Parameters, FComputeShaderUtils::GetGroupCount(IndirectionTextureDimensions, FIntVector(4)));


	////param swapping
	//you just assign, like, Parameters.My_Output_UAV = MyUAVs[pass % 3];


	//data is always uploaded when you do Lock / Unlock or when you create the buffer with a ResourceArray
	//	they are in vram after that


	//meanwhile you can just put your shader directly inside your module instead of another module
	//	and call ENQUEUE_RENDER_COMMAND at any point in your code to execute some stuff on render thread


	//	search for "Remind Me EvoPulseGaming" in discord ue4 for this above







};

