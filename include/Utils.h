#ifndef UTILS_H
#define UTILS_H

#define GOLDEN_ANGLE 2.39996322f
#define RAD_TO_DEG 57.2957795f
#define DEG_TO_RAD 0.0174532925f
#define HALF_PI 1.57079632679f
#define PI 3.1416f
#define TWO_PI 6.28318530718f
#define DEBUG

#include <string>
#include <sstream>

float id(float val);

template<typename T>
std::string ttos( T t )
{
   std::stringstream ss;
   std::string s;
   if(ss << t){
   ss >> s;
   return s;
   }
   return "";
}

template <typename T>
T stot( std::string s )
{
    std::stringstream ss( s );
    T t;
    ss >> t;
    return t;
}

inline static float fsign(float x){
    return (x > 0.f) ? 1.f : -1.f;
}
inline static int isign(float x){
    return (x > 0) ? 1 : (x==0) ? 0 : -1;
}

#include <SFML/Graphics.hpp>

namespace Utils{
    /*static int log10(int x){
        int res = 0;
        while(1){
            res++;
            x /= 10;
            if(x==0) break;
        }
        return res;
    }*/
    /*inline float function_inv(float x, const float a)
    {
        return (1+a)*x/(1+a*x);
    }*/
    inline bool inCircle(sf::Vector2f point, sf::Vector2f p_circle,
                            float radius)
    {
        return (point.x-p_circle.x)*(point.x-p_circle.x)+
            (point.y-p_circle.y)*(point.y-p_circle.y) < radius*radius;
    }
}


#include <map>


class TextureManager
{
    private :
        // The textures that are loaded
        static std::map<std::string, sf::Texture*> textures;

        // Constructor that is not used
        TextureManager();
    public:
        // Destructor which deletes the textures previously loaded
        ~TextureManager();

        // Loads the texture and returns a pointer to it
        // If it is already loaded, this function just returns it
        // If it cannot find the file, returns NULL
        static sf::Texture *getTexture(std::string str);
        //static sf::IntRect const getTextureRect(string str);
        static sf::IntRect getTextureRectBonus(int str);
        static void assignTextureRect(sf::Sprite & sprite, std::string name, int id_element);
};

class FontManager
{
   static std::map<std::string, sf::Font*> fonts;
   FontManager();
public:
   ~FontManager();
   static sf::Font *getFont(std::string str);
};



#endif
