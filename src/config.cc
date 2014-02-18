#include <cstring>
#include "config.hh"

using namespace tinyxml2;

config::config(){
    XMLDocument doc;
    doc.LoadFile("config/sysSetting.xml");
    XMLElement* element;
    for(element = doc.FirstChildElement("sysSetting")->FirstChildElement("attribute");
        element!=NULL;
        element=element->NextSiblingElement("attribute")){
        XMLElement* ele = element->FirstChildElement("name");
        if(strcmp(ele->GetText(),"nodePerRack")==0){
            _nodePerRack=atoi(ele->NextSiblingElement("value")->GetText());
        }else if(strcmp(ele->GetText(),"rackNum")==0){
            _rackNum=atoi(ele->NextSiblingElement("value")->GetText());
        }else if(strcmp(ele->GetText(),"ecN")==0){
            _ecN=atoi(ele->NextSiblingElement("value")->GetText());
        }else if(strcmp(ele->GetText(),"ecK")==0){
            _ecK=atoi(ele->NextSiblingElement("value")->GetText());
        }else if(strcmp(ele->GetText(),"replicaNum")==0){
            _replicaNum=atoi(ele->NextSiblingElement("value")->GetText());
        }else if(strcmp(ele->GetText(),"repPlaPolicy")==0){
            _repPlaPolicy=atoi(ele->NextSiblingElement("value")->GetText());
        }
    }
    _nodeNum=_rackNum*_nodePerRack;
}


