#include "yaml-cpp/yaml.h"

#include "TrainingInput.h"

bool TrainingInput::read(const std::string& fileName)
{

	// parse the atom/yaml...
    YAML::Node config;
    try 
    {
        config = YAML::Load(fileName);
    } 
    catch (YAML::Exception& e)
    {
        LOG_ERROR(("Exception loading file [%v] into YAML [%v]", filename, e.what()));
        return false;
    }

    for (YAML::const_iterator it=config.begin(); it!=config.end(); ++it) 
    {
    	TrainingFile pcapFile;
    	pcapFile.filename = (*it)["file"].as<std::string>();

    	for (YAML::const_iterator fls = (*it)["flows"].begin(); 
    	 		fls != (*it)["flows"].end(); ++fls)
    	{
    	 	TrainingFlow flow;
    	 	string src_ip = (*fls)["src_ip"].as<string>();
    	 	flow.tuple.src_port = (*fls)["src_port"].as<uint16_t>();
    	 	string dst_ip = (*fls)["dst_ip"].as<string>();
    	 	flow.tuple.src_port = (*fls)["dst_port"].as<uint16_t>();
    	 	string transport = (*fls)["transport"].as<string>();
    	 	string protocol = (*fls)["protocol"].as<string>();

    	 	


    	}

    }

}