#include "D3D11Renderer.h"
#include "GLRenderer.h"

int main() {
  std::cout << "Starting Build Test..." << std::endl;

  std::cout << "Testing D3D11Renderer instantiation..." << std::endl;
  D3D11Renderer d3dRenderer;

  std::cout << "Testing GLRenderer instantiation..." << std::endl;
  GLRenderer glRenderer;

  std::cout << "Build Test Completed Successfully." << std::endl;
  return 0;
}
