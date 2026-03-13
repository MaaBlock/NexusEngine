#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <shaderc/shaderc.hpp>

int main() {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetSourceLanguage(shaderc_source_language_hlsl);
    // options.SetHlslFunctionality1(true);
    
    std::string source = R"(
float4x4 mvp;
float4 VSMain(float3 pos : POSITION) : SV_POSITION {
    return mvp * float4(pos, 1.0);
}
)";

    auto result = compiler.CompileGlslToSpv(source, shaderc_vertex_shader, "test.hlsl", "VSMain", options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::cerr << "Compile Error: " << result.GetErrorMessage() << std::endl;
        return 1;
    }
    
    std::vector<uint32_t> spirv(result.cbegin(), result.cend());
    std::ofstream out("test.spv", std::ios::binary);
    out.write((char*)spirv.data(), spirv.size() * 4);
    std::cout << "Compiled test.spv (" << spirv.size() * 4 << " bytes)" << std::endl;
    return 0;
}
