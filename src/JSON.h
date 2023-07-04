#pragma once
/** @file */ //doxygen
/**
  Ten plik zawiera funkcje związane z biblioteką JSON
*/
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
using namespace nlohmann;
namespace mw
{
/**
 * Wczytuje obiekt JSON z pliku o podanej nazwie
 */
json openJSON(std::string filename){
  std::ifstream file ( filename );
  if ( !file ) {
      std::cerr << "File couldn't be opened!" <<std::endl;
      throw mw::fstreamFileNotOpened(filename);
  }
  return json::parse ( file );
}
}
