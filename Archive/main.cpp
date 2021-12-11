#include <iostream>
#include <filesystem>

int main()
{
//  gengine2d::NonCompressedArchive a;
//  if(!a.open("myFile.arc")) {
//    std::cout << "Error reading archive" << std::endl;
//  }

//  if(!a.extractFile("image.png", "otherImage.png")) {
//    std::cout << "Error extracting file image.png" << std::endl;
//  }

//  if(!a.extractFile("text.txt", "otherText.txt")) {
//    std::cout << "Error extracting file text.txt" << std::endl;
//  }

//  if(!a.addFile("image.png", "image.png")){
//    std::cout << "Error adding image.png" << std::endl;
//  }
//  if(!a.addFile("text.txt", "text.txt")) {
//    std::cout << "Error adding text.txt" << std::endl;
//  }

//  if(!a.save("myFile.arc")) {
//    std::cout << "Error saving" << std::endl;
//  }

//  gengine2d::NonCompressedArchive a;
//  if(!a.open("mymap")) {
//    std::cout << "Cannot open map!" << std::endl;
//  }

//  if(!a.extractFile("tileInfo.json", "theInfo.json")) {
//    std::cout << "Cannot extract tileInfo.json" << std::endl;
//  }

//  if(!a.extractFile("tileTexture.png", "theTexture.png")) {
//    std::cout << "Cannot extract tileTexture.png" << std::endl;
//  }

//  gengine2d::NonCompressedArchive a;
//  if(auto openRes = a.open("pack.spak");
//     openRes!= gengine2d::NonCompressedArchive::EOR_SUCCESS) {
//    std::cerr << "Cannot open archive! Error code: " << std::hex << "0x" << openRes  << std::endl;
//  }
  std::filesystem::path p("file.txt");
  std::cout << p.parent_path() << std::endl;

  return 0;
}
