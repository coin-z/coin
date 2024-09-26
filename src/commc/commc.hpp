/**
 * @brief obj_gen.hpp
*/
#include <string>
#include <memory>
#include <code_scan/code_decoder.hpp>


namespace coin
{
class ObjGen
{
public:
    ObjGen();
    ~ObjGen();

    bool loadFile(const std::string& file, const std::string tail = "obj.cpp");
    bool loadFile(const std::vector< std::string >& files);
    bool saveObjFile(const std::string& path);

    bool dumpObjFile(const std::vector<std::string>& files, const std::string& path);

    std::string target;
    bool is_drymodel;

private:
    std::vector<std::shared_ptr<coin::code::CodeDecoder>> proto_decoder_;
};

} // namespace coin

