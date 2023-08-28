/**
* \file DicoSynonymes.cpp
* \brief Le code des opérateurs du DicoSynonymes.
* \author IFT-2008, Olivier Caron
* \version 0.1
* \date juillet 2023
*
* Travail pratique numéro 3.
*
*/

#include "DicoSynonymes.h"

namespace TP3
{

    /**
    *\brief     Constructeur
    *
    *\post      Une instance vide de la classe a été initialisée
    *
    */
    DicoSynonymes::DicoSynonymes() : racine(0),nbRadicaux(0), groupesSynonymes(std::vector< std::list<NoeudDicoSynonymes*> >{}){
    }

    /**
      *\brief  Constructeur de dictionnaire à partir d'un fichier
      *
      *\pre    Il y a suffisament de mémoire
      *\pre    Le fichier est ouvert au préalable
      *
      *\post   Si le fichier est ouvert, l'instance de la classe a été initialisée à partir
      *        du fichier de dictionnaire. Sinon, on génère une classe vide.
      *
      *\exception bad_alloc s'il n'y a pas assez de mémoire
      * Cette méthode appelle chargerDicoSynonyme !
      */
    DicoSynonymes::DicoSynonymes(std::ifstream &fichier){
        if (!fichier.is_open())
        {
            racine = 0;
            nbRadicaux = 0;
            groupesSynonymes = std::vector< std::list<NoeudDicoSynonymes*> >{};
        }
        else
        {
            chargerDicoSynonyme(fichier);
        }
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

    /**
      *\brief     Destructeur.
      *
      *\post      Une instance de la classe est détruite.
      *
      */
    DicoSynonymes::~DicoSynonymes(){
    }

    /**
      *\brief     Ajouter un radical au dictionnaire des synonymes
      *\brief     tout en s’assurant de maintenir l'équilibre de l'arbre.
      *
      *\pre    Il y a suffisament de mémoire.
      *
      *\post   Le mot est ajouté au dictionnaire des synonymes.
      *
      *\exception bad_alloc si il n'y a pas suffisament de mémoire.
      *\exception logic_error si le radical existe déjà.
      */
    void DicoSynonymes::ajouterRadical(const std::string& motRadical){
        if (radicalDansDico(motRadical))
            throw std::logic_error("ajouterRadical : Le radical est déjà dans le dictionnaire");

        auxajouterRadical(motRadical, racine);
    }

    /**
     *\brief  Ajouter une flexion (motFlexion) d'un radical (motRadical) à sa liste de flexions.
     *
     *\pre    Il y a suffisament de mémoire.
     *
     *\post   La flexion est ajoutée au dictionnaire des synonymes.
     *
     *\exception bad_alloc si il n'y a pas suffisament de mémoire.
     *\exception logic_error si motFlexion existe déjà ou motRadical n'existe pas.
     */
    void DicoSynonymes::ajouterFlexion(const std::string& motRadical, const std::string& motFlexion){
        if (!radicalDansDico(motRadical))
            throw std::logic_error("ajouterFlexion : Le radical n'est pas dans le dictionnaire");

        // Note au correcteur : Pour le logic_error si la flexion est déjà dans le radical,
        // voir dans la fonction auxAjouterFlexion

        auxAjouterFlexion(motRadical, motFlexion, racine);
    }

    /**
      *\brief  Ajouter un synonyme (motSynonyme) d'un radical (motRadical)
      *\brief  à un de ses groupes de synonymes.
      *
      *\pre    Le radical se trouve déjà dans le dictionnaire
      *\       Il y a suffisament de mémoire
      *
      *\post   Le synonyme est ajouté au dictionnaire des synonymes
      *\       Si numGroupe vaut –1, le synonyme est ajouté dans un nouveau groupe de synonymes
      *\       et retourne le numéro de ce nouveau groupe dans numgroupe par référence.
      *
      *\exception bad_alloc si il n'y a pas suffisament de mémoire
      *\exception logic_error si motSynonyme est déjà dans la liste des synonymes du motRadical
      *\exception logic_error si numGroupe n'est pas correct ou motRadical n'existe pas.
      */
    void DicoSynonymes::ajouterSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe){
        if (numGroupe != - 1 && numGroupe > groupesSynonymes.size() - 1)
            throw std::logic_error("ajouterSynonyme : Le numéro de groupe est invalide");
        if (!radicalDansDico(motRadical))
            throw std::logic_error("ajouterSynonyme : Le radical n'est pas dans le dictionnaire");
        if (!radicalDansDico(motSynonyme))
            ajouterRadical(motSynonyme);

        std::vector<NoeudDicoSynonymes*> arbre = parcoursPreOrdre();
        NoeudDicoSynonymes* adresseRadical;
        NoeudDicoSynonymes* adresseSynonyme;

        //Détermine les adresses des mots radical et synonymes
        for(auto noeud : arbre)
        {
            if (noeud->radical == motRadical) adresseRadical = noeud;
            if(noeud->radical == motSynonyme) adresseSynonyme = noeud;
        }

        // Si le groupe n'existe pas faire une nouvelle liste et l'ajouter au vecteur groupeSynonymes
        if (numGroupe == -1)
        {
            std::list<NoeudDicoSynonymes*> nouvelleListe {adresseRadical, adresseSynonyme};
            groupesSynonymes.push_back(nouvelleListe);
            numGroupe = groupesSynonymes.size() - 1;
        }
        // sinon déterminer si le radical et le synonyme sont présents dans le groupe de synonymes
        else
        {
            bool radicalPresent = false;
            for(auto adresse : groupesSynonymes[numGroupe])
            {
                if (adresse == adresseRadical) radicalPresent = true;
                // si le synonyme est déjà présent on lance une erreur
                if (adresse == adresseSynonyme) throw std::logic_error("ajouterSynonyme : Le synonyme est déjà présent dans le groupe");
            }

            // si le radical n'est pas présent, on l'ajoute
            if (!radicalPresent)
            {
                groupesSynonymes[numGroupe].push_back(adresseRadical);
            }
            // ajout du synonyme
            groupesSynonymes[numGroupe].push_back(adresseSynonyme);
        }

        bool numEstPresent = false;
        for(auto num : adresseRadical->appSynonymes)
        {
            if (numGroupe == num) numEstPresent = true;
        }
        if(!numEstPresent)
        {
            // si le numéro de groupe synonyme n'est pas présent dans l'appSynonymes du noeud, on l'ajoute
            adresseRadical->appSynonymes.push_back(numGroupe);
        }
        // ajout du groupe de synonyme au synonyme
        adresseSynonyme->appSynonymes.push_back(numGroupe);
    }

    /**
     *\brief     Supprimer un radical du dictionnaire des synonymes
     *\brief     tout en s’assurant de maintenir l'équilibre de l'arbre.
     *
     *\pre    motRadical fait parti de l'arbre.
     *
     *\post   Si le radical appartient au dictionnaire, on l'enlève et on équilibre.
     *\       Il faut libérer la liste des synonymes
     *\       du radical en question ainsi que sa liste des flexions.
     *\       Il faut aussi s’assurer d’éliminer tout pointeur
     *\       pointant sur le nœud contenant le radical.
     *
     *\exception logic_error si l'arbre est vide ou motRadical n'existe pas.
     */
    void DicoSynonymes::supprimerRadical(const std::string& motRadical){
        if (estVide()) throw std::logic_error("supprimerRadical : L'arbre est vide");
        if (!radicalDansDico(motRadical))
            throw std::logic_error("supprimerRadical : Le radical n'est pas dans le dictionnaire");
        auxSupprimerRadical(motRadical, racine);
    }

    /**
     *\brief   Supprimer une flexion (motFlexion) d'un radical
     *\brief   (motRadical) de sa liste de flexions.
     *
     *\pre    motRadical et motFlexion font parti de l'arbre.
     *
     *\post   La flexion est enlevée du dictionnaire des synonymes.
     *
     *\exception logic_error si si l'arbre est vide ou
     *\exception motFlexion n'existe pas ou motRadical n'existe pas.
     */
    void DicoSynonymes::supprimerFlexion(const std::string& motRadical, const std::string& motFlexion){
        if (estVide()) throw std::logic_error("supprimerFlexion : L'arbre est vide");
        if (!radicalDansDico(motRadical))
            throw std::logic_error("supprimerFlexion : Le radical n'est pas dans le dictionnaire");

        // Note au correcteur : Pour le logic_error si la flexion n'est pas dans le radical,
        // voir dans la fonction auxSupprimerFlexion

        auxSupprimerFlexion(motRadical, motFlexion, racine);
    }

    /**
      *\brief   Retirer motSynonyme faisant partie du numéro de groupe numGroupe du motRadical.
      *
      *\pre    motRadical et motSynonyme existent et motRadical
      *\pre    a une appartenance au groupe numGroupe
      *
      *\post      Le synonyme est enlevé du dictionnaire des synonymes.
      *
      *\exception logic_error si motSynonyme ou motRadical ou numGroupe n'existent pas.
      */
    void DicoSynonymes::supprimerSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe){
        if (!radicalDansDico(motRadical))
            throw std::logic_error("supprimerSynonyme : Le radical n'est pas dans le dictionnaire");
        if (!radicalDansDico(motSynonyme))
            throw std::logic_error("supprimerSynonyme : Le synonyme n'est pas dans le dictionnaire");
        if (numGroupe > groupesSynonymes.size() - 1)
            throw std::logic_error("supprimerSynonyme : Le numéro de groupe est invalide");

        bool radicalEstPresent = false;
        // on détermine si le radical est présent dans le groupe de synonymes
        for(auto adresse : groupesSynonymes[numGroupe])
        {
            if(adresse->radical == motRadical) radicalEstPresent = true;
        }

        if (radicalEstPresent)
        {
            // si le radical est présent, on détermine quelle adresse enlever
            NoeudDicoSynonymes* adresseAEnlever;
            for(auto adresse : groupesSynonymes[numGroupe])
            {
                if(adresse->radical == motSynonyme) adresseAEnlever = adresse;
            }
            groupesSynonymes[numGroupe].remove(adresseAEnlever);

            std::vector<int>::iterator it = adresseAEnlever->appSynonymes.begin();
            for (int i = 0; i < adresseAEnlever->appSynonymes.size(); ++i) {
                if(adresseAEnlever->appSynonymes[i] == numGroupe)
                {
                    std::next(it, i);
                    adresseAEnlever->appSynonymes.erase(it);
                }
            }
        }
        else
        {
            throw std::logic_error("supprimerSynonyme : Le radical n'appartient pas au groupe de synonymes");
        }
    }

    /**
      *\brief     Vérifier si le dictionnaire est vide
      *
      *\post      Le dictionnaire est inchangée
      *
      */
    bool DicoSynonymes::estVide() const{
        return nbRadicaux == 0;
    }

    /**
     * \brief Retourne le nombre de radicaux dans le dictionnaire
     *
     * \post Le dictionnaire reste inchangé.
     *
     */
    int DicoSynonymes::nombreRadicaux() const{
        return nbRadicaux;
    }

    /**
     * \brief Retourne le radical du mot entré en paramètre
     *
     * \pre mot est dans la liste des flexions d'un radical
     *
     * \post Le dictionnaire reste inchangé.
     * \post Le radical est retournée.
     *
     * \exception logic_error si l'arbre est vide
     * \exception logic_error si la flexion n'est pas dans la liste de flexions du radical
     *
     */
    std::string DicoSynonymes::rechercherRadical(const std::string& mot) const
    {
        if (estVide()) throw std::logic_error("rechercherRadical : L'arbre est vide");

        std::vector<NoeudDicoSynonymes*> arbre = parcoursPreOrdre();
        std::vector<float> degresDeSimilitude;

        for (auto noeud : arbre) {
            // j'ajoute le degré de similitude fait par ma fonction similitude pour chaque mot de l'arbre et le mot entré en paramètre
            degresDeSimilitude.push_back(similitude(noeud->radical, mot));
        }

        int posSimilitudeMax = 0;
        // Je trouve le degré de similitude le plus élevé de mon vecteur
        auto similitudeMax = *std::max_element(std::begin(degresDeSimilitude), std::end(degresDeSimilitude));

        for (int i = 0; i < degresDeSimilitude.size(); ++i) {
            // Je détermine la position du degré maximum dans le vecteur
            if (degresDeSimilitude[i] == similitudeMax)
                posSimilitudeMax = i;
        }

        bool flexionEstPresente = false;

        // J'assigne le noeud ayant le degré de similitude le plus élevé
        auto noeud = arbre[posSimilitudeMax];

        for (auto flexion : noeud->flexions) {
            // je cherche si le mot est dans la liste de flexion du noeud
            if (flexion == mot) return noeud->radical;
        }
        // si le mot n'est pas dans la liste de flexions, je lance une erreur
        throw std::logic_error("rechercherRadical : Le mot n'est dans aucune liste de flexions");
    }

    /**
      * \brief Retourne un réel entre 0 et 1 qui représente le degré de similitude entre mot1 et mot2 où
      *        0 représente deux mots complétement différents et 1 deux mots identiques.
      *        Vous pouvez utiliser par exemple la distance de Levenshtein, mais ce n'est pas obligatoire !
      *
      * \post Un réel entre 0 et 1 est retourné
      * \post Le dictionnaire reste inchangé.
      *
      */
    float DicoSynonymes::similitude(const std::string& mot1, const std::string& mot2) const
    {
        // retourne 1 - la distance de Levenstein entre les 2 mots divisé par la longueur du mot le plus long
        return 1 - (distanceLevenstein(mot1, mot2) / std::max(mot1.size(), mot2.size()));
    }

    /**
      * \brief Donne le nombre de cellules de appSynonymes.
      *
      *
      * \post Le nombre de cellules de appSynonymes pour le radical entré en paramètre est retourné
      * \post Le dictionnaire reste inchangé.
      *
      */
    int DicoSynonymes::getNombreSens(std::string radical) const{
        return auxGetNombreSens(radical, racine);
    }

    /**
      * \brief Donne le premier synonyme du groupe de synonyme de l'emplacement entrée en paramètre.
      *
      *
      * \post un string correspondant au premier synonyme est imprimé
      * \post Le dictionnaire reste inchangé.
      *
      */
    std::string DicoSynonymes::getSens(std::string radical, int position) const{
        int groupe = 0;

        // détermine de quel groupe de synonyme il s'agit
        for(auto liste : groupesSynonymes)
        {
            for(auto adresse : liste)
            {
                if(adresse->radical == radical)
                {
                    groupe = adresse->appSynonymes[position];
                }
            }
        }

        std::vector<NoeudDicoSynonymes*> sens;

        // converti la liste en vecteur. J'ai fait cela car j'avais toujours des segfaults en essayant
        // de le faire avec un iterateur dans la liste
        for (auto &s : groupesSynonymes[groupe])
        {
            sens.push_back(s);
        }

        // si le premier élément du vecteur est équivalent au mot, on retourne le prochain élément
        if (sens[0]->radical == radical)
        {
            return sens[1]->radical;
        }

        return groupesSynonymes[groupe].front()->radical;
    }

    /**
    * \brief Donne tous les synonymes du mot entré en paramètre du groupeSynonyme du parametre position
    *
    *
    * \post un vecteur est retourné avec tous les synonymes
    * \post Le dictionnaire reste inchangé.
    *
    */
    std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const{
        int groupe = 0;

        // détermine de quel groupe de synonyme il s'agit
        for(auto liste : groupesSynonymes)
        {
            for(auto adresse : liste)
            {
                if(adresse->radical == radical)
                {
                    groupe = adresse->appSynonymes[position];
                }
            }
        }
        std::vector<std::string> synonymes;

        for(auto adresse : groupesSynonymes[groupe])
        {
            // pour chaque élément du groupe de synonyme, si ce n'est pas le même mot, je met le mot dans le vecteur
            if(adresse->radical != radical) synonymes.push_back(adresse->radical);
        }

        return synonymes;
    }

    /**
      * \brief Donne toutes les flexions du mot entré en paramètre
      *
      *
      * \post un vecteur est retourné avec toutes les flexions
      * \post Le dictionnaire reste inchangé.
      *
      */
    std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const
    {
        return auxGetFlexions(radical, racine);
    }

    //Mettez l'implantation des autres méthodes (surtout privées) ici.

    /**
    * \fn void auxajouterRadical(const std::string &motRadical, DicoSynonymes::NoeudDicoSynonymes *&root)
    * \brief Fonction récursive servant à ajouter un radical à la bonne place dans l'arbre AVL tout en le gardant équilibré
    * \param[in] motRadical le mot à ajouter à l'arbre AVL
    * \param[in] root la racine de l'arbre ou du sous-arbre où ajouter le mot
    */
    void DicoSynonymes::auxajouterRadical(const std::string &motRadical, DicoSynonymes::NoeudDicoSynonymes *&root) {
        if (!root)
        {
            root = new NoeudDicoSynonymes(motRadical);
            nbRadicaux++;
            return;
        }
        if (motRadical > root->radical) auxajouterRadical(motRadical, root->droit);
        else if (motRadical < root->radical) auxajouterRadical( motRadical, root->gauche);

        equilibrer(root);
    }

    /**
      * \fn void auxAjouterFlexion(const std::string &motRadical, const std::string &motFlexion, DicoSynonymes::NoeudDicoSynonymes *&root)
      * \brief Fonction récursive servant à ajouter une flexion au bon noeud de l'arbre AVL
      * \param[in] motRadical le mot où ajouter la flexion
      * \param[in] motFlexion la flexion à ajouter
      * \param[in] root la racine de l'arbre ou du sous-arbre où ajouter la flexion
      */
    void DicoSynonymes::auxAjouterFlexion(const std::string &motRadical, const std::string &motFlexion,
                                          DicoSynonymes::NoeudDicoSynonymes *&root) {
        if (root->radical == motRadical)
        {
            for (auto flexion : root->flexions) {
                if(motFlexion == flexion) throw std::logic_error("ajouterFlexion : La flexion est déjà présente dans le radical");
            }
            root->flexions.push_back(motFlexion);
        }
        else if (motRadical > root->radical) auxAjouterFlexion(motRadical, motFlexion, root->droit);
        else if (motRadical < root->radical) auxAjouterFlexion( motRadical, motFlexion, root->gauche);
    }

    /**
  * \fn void auxSupprimerRadical(const std::string &motRadical, DicoSynonymes::NoeudDicoSynonymes *&root)
  * \brief Fonction récursive servant à supprimer un radical à la bonne place dans l'arbre AVL tout en le gardant équilibré
  * \param[in] motRadical le mot à supprimer
  * \param[in] root la racine de l'arbre ou du sous-arbre où supprimer le radical
  */
    void DicoSynonymes::auxSupprimerRadical(const std::string &motRadical, DicoSynonymes::NoeudDicoSynonymes *&root) {
        if (motRadical < root->radical) auxSupprimerRadical(motRadical, root->gauche);
        else if (root->radical < motRadical) auxSupprimerRadical(motRadical, root->droit);
        // si le radical a deux enfants nous devons enlever le successeur minimal à droite
        else if (root->gauche != 0 && root->droit != 0)
        {
            enleverSuccMinDroit(root);
        }
        else
        {
            // le noeud à détruire
            auto vieuxNoeud  = root;
            // si le noeud a un enfant à gauche on l'assigne sinon on prend le droit
            root = (root->gauche != 0) ? root->gauche : root->droit;
            delete vieuxNoeud;
            --nbRadicaux;
        }
        equilibrer(root);
    }

    /**
    * \fn void auxSupprimerFlexion(const std::string &motRadical, const std::string &motFlexion, DicoSynonymes::NoeudDicoSynonymes *&root)
    * \brief Fonction récursive servant à supprimer une flexion au bon noeud de l'arbre
    * \param[in] motRadical le mot où supprimer la flexion
    * \param[in] motFlexion la flexion à supprimer
    * \param[in] root la racine de l'arbre ou du sous-arbre où supprimer la flexion
*/
    void DicoSynonymes::auxSupprimerFlexion(const std::string &motRadical, const std::string &motFlexion,
                                            DicoSynonymes::NoeudDicoSynonymes *&root) {
        if (root->radical == motRadical)
        {
            for (auto flexion : root->flexions) {
                if(motFlexion == flexion)
                {
                    root->flexions.remove(motFlexion);
                    return;
                }
            }
            throw std::logic_error("ajouterFlexion : La flexion n'est pas présente dans le radical");
        }
        else if (motRadical > root->radical) auxSupprimerFlexion(motRadical, motFlexion, root->droit);
        else if (motRadical < root->radical) auxSupprimerFlexion( motRadical, motFlexion, root->gauche);

    }

    /**
    * \fn int hauteur(DicoSynonymes::NoeudDicoSynonymes *root) const
    * \brief Fonction retournant la hauteur d'un noeud
    * \param[in] root le noeud où aller chercher la hauteur
    * \post L'arbre reste inchangé.
    * \return int la hauteur du noeud
    */
    int DicoSynonymes::hauteur(DicoSynonymes::NoeudDicoSynonymes *root) const {
        if (!root) return -1;
        return root->hauteur;
    }

    /**
   * \fn bool radicalDansDico(const std::string &motRadical) const
   * \brief Fonction déterminant si un radical se trouve dans le dictionnaire
   * \param[in] motRadical le mot à chercher
   * \post L'arbre reste inchangé.
   * \return bool vrai si le mot est présent, false sinon
    */
    bool DicoSynonymes::radicalDansDico(const std::string &motRadical) const {
        return auxradicalDansDico(motRadical, racine);
    }

    /**
    * \fn bool auxradicalDansDico(const std::string &motRadical, NoeudDicoSynonymes *root) const
    * \brief Fonction récursive déterminant si un radical se trouve dans le dictionnaire
    * \param[in] motRadical le mot à chercher
    * \param[in] root le noeud où aller chercher le mot
    * \post L'arbre reste inchangé.
    * \return bool vrai si le mot est présent, false sinon
    */
    bool DicoSynonymes::auxradicalDansDico(const std::string &motRadical, NoeudDicoSynonymes *root) const {
        if (!root) return false;
        if (motRadical == root->radical) return true;
        if (motRadical < root->radical) return auxradicalDansDico(motRadical, root->gauche);
        return auxradicalDansDico(motRadical, root->droit);
    }

    /**
    * \fn void equilibrer(DicoSynonymes::NoeudDicoSynonymes *&root)
    * \brief Fonction servant à équilibrer l'arbre AVL
    * \param[in] root le noeud à équilibrer
    */
    void DicoSynonymes::equilibrer(DicoSynonymes::NoeudDicoSynonymes *&root) {
         if (debalancementAGauche(root)) // déséquilibre à gauche
         {
             if (sousArbrePencheADroite(root->gauche)) // noeud sous-critique penche à droite
             {
                 zigZagGauche(root);
             }
             else                                            // noeud sous-critique penche à gauche
             {
                 zigZigGauche(root);
             }
         }
        else if (debalancementADroite(root)) // déséquilibre à droite
         {
            if (sousArbrePencheAGauche(root->droit)) // noeud sous-critique penche à gauche
            {
                zigZagDroit(root);
            }
            else{                                          // noeud sous-critique penche à droite
                zigZigDroit(root);
            }
         }
        else
         {
            // mise à jour de la hauteur
             if (root != 0)
                 root->hauteur = 1 + std::max(hauteur(root->gauche), hauteur(root->droit));
         }
    }

    /**
    * \fn void zigZigGauche(NoeudDicoSynonymes *&noeud)
    * \brief Fonction servant à équilibrer l'arbre AVL en faisant une rotation zigzigGauche
    * \param[in] noeud le noeud à rotationner
    */
    void DicoSynonymes::zigZigGauche(NoeudDicoSynonymes *&noeud) {
        auto nouvelleRacine = noeud->gauche;
        noeud->gauche = nouvelleRacine->droit;
        nouvelleRacine->droit = noeud;
        noeud->hauteur = 1 + std::max(hauteur(noeud->gauche), hauteur(noeud->droit));
        nouvelleRacine->hauteur = 1 + std::max(hauteur(nouvelleRacine->gauche), hauteur(nouvelleRacine->droit));
        noeud = nouvelleRacine;
    }

    /**
    * \fn void zigZigDroit(NoeudDicoSynonymes *&noeud)
    * \brief Fonction servant à équilibrer l'arbre AVL en faisant une rotation zigzigDroit
    * \param[in] noeud le noeud à rotationner
    */
    void DicoSynonymes::zigZigDroit(NoeudDicoSynonymes *&noeud) {
        auto nouvelleRacine = noeud->droit;
        noeud->droit = nouvelleRacine->gauche;
        nouvelleRacine->gauche = noeud;
        noeud->hauteur = 1 + std::max(hauteur(noeud->droit), hauteur(noeud->gauche));
        nouvelleRacine->hauteur = 1 + std::max(hauteur(nouvelleRacine->droit), hauteur(nouvelleRacine->gauche));
        noeud = nouvelleRacine;
    }

    /**
    * \fn void zigZagGauche(NoeudDicoSynonymes *&noeud)
    * \brief Fonction servant à équilibrer l'arbre AVL en faisant une rotation zigZagGauche
    * \param[in] noeud le noeud à rotationner
    */
    void DicoSynonymes::zigZagGauche(DicoSynonymes::NoeudDicoSynonymes *&noeud) {
        zigZigDroit(noeud->gauche);
        zigZigGauche(noeud);
    }

    /**
    * \fn void zigZagDroit(NoeudDicoSynonymes *&noeud)
    * \brief Fonction servant à équilibrer l'arbre AVL en faisant une rotation zigZagDroit
    * \param[in] noeud le noeud à rotationner
    */
    void DicoSynonymes::zigZagDroit(DicoSynonymes::NoeudDicoSynonymes *&noeud) {
        zigZigGauche(noeud->droit);
        zigZigDroit(noeud);
    }

    /**
    * \fn bool debalancementAGauche(NoeudDicoSynonymes *&noeud) const
    * \brief Fonction déterminant si le noeud a un débalancement à gauche
    * \param[in] noeud le noeud à vérifier
    * \post L'arbre reste inchangé.
    * \return bool vrai s'il y a un débalancement à gauche, faux sinon
    */
    bool DicoSynonymes::debalancementAGauche(DicoSynonymes::NoeudDicoSynonymes *noeud) const {
        if(!noeud) return false;
        return 1 < hauteur(noeud->gauche) - hauteur(noeud->droit);
    }

    /**
    * \fn bool debalancementADroite(NoeudDicoSynonymes *&noeud) const
    * \brief Fonction déterminant si le noeud a un débalancement à droite
    * \param[in] noeud le noeud à vérifier
    * \post L'arbre reste inchangé.
    * \return bool vrai s'il y a un débalancement à droite, faux sinon
    */
    bool DicoSynonymes::debalancementADroite(DicoSynonymes::NoeudDicoSynonymes *noeud) const {
        if(!noeud) return false;
        return 1 < hauteur(noeud->droit) - hauteur(noeud->gauche);
    }

    /**
    * \fn bool sousArbrePencheADroite(NoeudDicoSynonymes *&noeud) const
    * \brief Fonction déterminant si le noeud a un sous-arbre penchant à droite
    * \param[in] noeud le noeud à vérifier
    * \post L'arbre reste inchangé.
    * \return bool vrai si le sous-arbre penche à droite, faux sinon
    */
    bool DicoSynonymes::sousArbrePencheADroite(DicoSynonymes::NoeudDicoSynonymes *noeud) const {
        if(!noeud) return false;
        return hauteur(noeud->gauche) < hauteur(noeud->droit);
    }

    /**
    * \fn bool sousArbrePencheAGauche(NoeudDicoSynonymes *&noeud) const
    * \brief Fonction déterminant si le noeud a un sous-arbre penchant à gauche
    * \param[in] noeud le noeud à vérifier
    * \post L'arbre reste inchangé.
    * \return bool vrai si le sous-arbre penche à gauche, faux sinon
    */
    bool DicoSynonymes::sousArbrePencheAGauche(DicoSynonymes::NoeudDicoSynonymes *noeud) const {
        if(!noeud) return false;
        return hauteur(noeud->gauche) > hauteur(noeud->droit);
    }

    /**
    * \fn bool enleverSuccMinDroit(NoeudDicoSynonymes *&noeud)
    * \brief Fonction servant à enlever le successeur minimal à droite
    * \param[in] noeud le noeud à changer
    */
    void DicoSynonymes::enleverSuccMinDroit(DicoSynonymes::NoeudDicoSynonymes *noeud) {
        auto noeudTemporaire = noeud->droit;
        auto parent = noeud;

        //Je vais chercher le successeur minimal donc le neoud le plus en bas à gauche
        while(noeudTemporaire->gauche != 0)
        {
            parent = noeudTemporaire;
            noeudTemporaire = noeudTemporaire->gauche;
        }

        // Je swap les deux noeuds
        noeud->radical = noeudTemporaire->radical;

        // Je supprime le bon noeud
        if (noeudTemporaire == parent->gauche)
            auxSupprimerRadical(noeudTemporaire->radical, parent->gauche );
        else
            auxSupprimerRadical(noeudTemporaire->radical, parent->droit);
    }

    /**
    * \fn bool auxGetNombreSens(const std::string &motRadical, NoeudDicoSynonymes *root) const
    * \brief Fonction récursive servant à chercher le nombre de sens d'un mot
    * \param[in] motRadical le mot à chercher
    * \param[in] root le noeud où aller chercher le mot
    * \post L'arbre reste inchangé.
    * \return int le nombre de sens du mot
    */
    int DicoSynonymes::auxGetNombreSens(const std::string &motRadical, DicoSynonymes::NoeudDicoSynonymes *root) const {
        int nbSens;
        if (root->radical == motRadical)
        {
            return root->appSynonymes.size();
        }
        else if (motRadical > root->radical) nbSens = auxGetNombreSens(motRadical, root->droit);
        else if (motRadical < root->radical) nbSens = auxGetNombreSens( motRadical, root->gauche);

        return nbSens;
    }

    /**
    * \fn  std::vector<std::string> auxGetFlexions(const std::string &motRadical, NoeudDicoSynonymes *root) const
    * \brief Fonction récursive servant à chercher les flexions d'un mot
    * \param[in] motRadical le mot à chercher pour retourner sa liste de flexions
    * \param[in] root le noeud où aller chercher le mot
    * \post L'arbre reste inchangé.
    * \return std::vector<std::string> le vecteur de toutes les flexions du mot radical
    */
    std::vector<std::string> DicoSynonymes::auxGetFlexions(const std::string& motRadical, DicoSynonymes::NoeudDicoSynonymes *root) const {
        std::vector<std::string> flexions;
        if (root->radical == motRadical)
        {
            for (auto flexion : root->flexions) {
                flexions.push_back(flexion);
            }
            return flexions;
        }
        else if (motRadical > root->radical) flexions = auxGetFlexions(motRadical, root->droit);
        else if (motRadical < root->radical) flexions = auxGetFlexions( motRadical, root->gauche);

        return flexions;
    }

    /**
    * \fn std::vector<DicoSynonymes::NoeudDicoSynonymes*> parcoursPreOrdre() const
    * \brief Fonction retournant tous les pointeurs de noeuds dans un vecteur. Les éléments ne sont pas ordonnés
    * \post L'arbre reste inchangé.
    * \return std::vector<DicoSynonymes::NoeudDicoSynonymes*> le vecteur de toutes les adresses de l'Arbre dans un parcours pré-ordre
    */
    std::vector<DicoSynonymes::NoeudDicoSynonymes*> DicoSynonymes::parcoursPreOrdre() const {

            std::vector<NoeudDicoSynonymes*> accumulateur;
            auxParcoursPreOrdre(racine, accumulateur);
            return accumulateur;
    }

    /**
    * \fn  void auxParcoursPreOrdre(DicoSynonymes::NoeudDicoSynonymes *sousArbre,
                                            std::vector<NoeudDicoSynonymes *> &accumulateur) const
    * \brief Fonction récursive accumulant tous les pointeurs de noeuds dans un vecteur. Les éléments ne sont pas ordonnés
    * \param[in] sousArbre le noeud à insérer dans le vecteur
    * \param[in] accumulateur le vecteur de noeuds
    * \post L'arbre reste inchangé.
    */
    void DicoSynonymes::auxParcoursPreOrdre(DicoSynonymes::NoeudDicoSynonymes *sousArbre,
                                            std::vector<NoeudDicoSynonymes *> &accumulateur) const {
            if (!sousArbre) return;

            accumulateur.push_back(sousArbre);
            auxParcoursPreOrdre(sousArbre->gauche, accumulateur);
            auxParcoursPreOrdre(sousArbre->droit, accumulateur);
    }

    /**
    * \fn  float distanceLevenstein(Dconst std::string &mot1, const std::string &mot2) const
    * \brief Fonction calculant la distance entre deux mots
    * \param[in] mot1 le premier mot à comparer
    * \param[in] mot2 le deuxième mot à comparer
    * \post L'arbre reste inchangé.
    * \return float la similitude entre les 2 mots : 1 == mots identiques 0 == mots complètement différents
    */
    float DicoSynonymes::distanceLevenstein(const std::string &mot1, const std::string &mot2) const {
        const std::size_t len1 = mot1.size(), len2 = mot2.size();
        std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));

        d[0][0] = 0;
        for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
        for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

        for(unsigned int i = 1; i <= len1; ++i)
            for(unsigned int j = 1; j <= len2; ++j)
                d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (mot1[i - 1] == mot2[j - 1] ? 0 : 1) });
        return d[len1][len2];
    }


}//Fin du namespace