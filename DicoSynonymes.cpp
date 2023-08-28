/**
* \file DicoSynonymes.cpp
* \brief Le code des opérateurs du DicoSynonymes.
* \author IFT-2008, Étudiant(e)
* \version 0.1
* \date juillet 2023
*
* Travail pratique numéro 3.
*
*/

#include "DicoSynonymes.h"

namespace TP3
{

    DicoSynonymes::DicoSynonymes(){
    }

    DicoSynonymes::DicoSynonymes(std::ifstream &fichier){
    }

	// Méthode fournie
	void DicoSynonymes::chargerDicoSynonyme(std::ifstream& fichier)
	{
        if (!fichier.is_open())
            throw std::logic_error("DicoSynonymes::chargerDicoSynonyme: Le fichier n'est pas ouvert !");

		std::string ligne;
		std::string buffer;
		std::string radical;
		int cat = 1;

		while (!fichier.eof()) // tant qu'on peut lire dans le fichier
		{
            std::getline(fichier, ligne);

			if (ligne == "$")
			{
				cat = 3;
				std::getline(fichier, ligne);
			}
			if (cat == 1)
			{
				radical = ligne;
				ajouterRadical(radical);
				cat = 2;
			}
			else if (cat == 2)
			{
				std::stringstream ss(ligne);
				while (ss >> buffer)
					ajouterFlexion(radical, buffer);
				cat = 1;
			}
			else
			{
				std::stringstream ss(ligne);
				ss >> radical;
				ss >> buffer;
				int position = -1;
				ajouterSynonyme(radical, buffer, position);
				while (ss >> buffer)
					ajouterSynonyme(radical, buffer, position);
			}
		}
	}

    DicoSynonymes::~DicoSynonymes(){
    }

    void DicoSynonymes::ajouterRadical(const std::string& motRadical){
    }

    void DicoSynonymes::ajouterFlexion(const std::string& motRadical, const std::string& motFlexion){
    }

    void DicoSynonymes::ajouterSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe){
    }

    void DicoSynonymes::supprimerRadical(const std::string& motRadical){
    }

    void DicoSynonymes::supprimerFlexion(const std::string& motRadical, const std::string& motFlexion){
    }

    void DicoSynonymes::supprimerSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe){
    }

    bool DicoSynonymes::estVide() const{
        return true;
    }

    int DicoSynonymes::nombreRadicaux() const{
        return 0;
    }

    std::string DicoSynonymes::rechercherRadical(const std::string& mot) const
    {
        return "";
    }

    float DicoSynonymes::similitude(const std::string& mot1, const std::string& mot2) const
    {
        return 0.0;;
    }

    int DicoSynonymes::getNombreSens(std::string radical) const{
        return 0;
    }

    std::string DicoSynonymes::getSens(std::string radical, int position) const{
        return "";
    }

    std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const{
        std::vector<std::string> synonymes;
        return synonymes;
    }

    std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const
    {
        std::vector<std::string> flexions;
        return flexions;
    }

	//Mettez l'implantation des autres méthodes (surtout privées) ici.

}//Fin du namespace