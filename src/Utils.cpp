#include "../include/Utils.h"

float id(float x){return x;}

TextureManager::TextureManager()
{
}

std::map<std::string, sf::Texture*> TextureManager::textures;

sf::Texture* TextureManager::getTexture(std::string str)
{

   // See if we have already loaded this texture
   if(textures.find(str) != textures.end())
      return textures[str];

   // Haven't loaded it yet, time to create it
   sf::Texture *texture = new sf::Texture();
   if(texture->loadFromFile(str))
   {
      textures[str] = texture;
      return textures[str];
   }
   else
   {
      // Could not load the file
      delete texture;
      return NULL;
   }

}

TextureManager::~TextureManager()
{

   // Delete all of the textures we used
   sf::Texture *texture;
   std::map<std::string, sf::Texture*>::iterator iter = textures.begin();
   while(iter != textures.end())
   {
      texture = iter->second;
      delete texture;
      iter++;
   }

}

/*sf::IntRect const TextureManager::getTextureRect(string str)
{
    if(str == "bonus+life1"){
        return sf::IntRect(0,0,20,20);
    }else if(str == "bonus+shield1"){
        return sf::IntRect(20,0,20,20);
    }
    // unfounded
    return sf::IntRect(0,0,0,0);
}*/


void TextureManager::assignTextureRect(sf::Sprite & sprite, std::string name, int e){
    /*sprite.setTexture(*TextureManager::getTexture(name));
    if(name == "ressources/img/bullets1.png"){
        if(e == 1){
            sprite.setTextureRect(sf::IntRect(0,0,12,20));
        }
    }*/
}

// font manager ---

FontManager::FontManager(){}

std::map<std::string, sf::Font*> FontManager::fonts;

sf::Font* FontManager::getFont(std::string str)
{
    if(fonts.find(str) != fonts.end())
        return fonts[str];
    sf::Font *font = new sf::Font();
    if(font->loadFromFile(str)){
      fonts[str] = font;
      return fonts[str];
   }else{
      delete font;
      return NULL;
   }

}

FontManager::~FontManager()
{
   sf::Font *font;
   std::map<std::string, sf::Font*>::iterator iter = fonts.begin();
   while(iter != fonts.end())
   {
      font = iter->second;
      delete font;
      iter++;
   }

}
