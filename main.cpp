
/*************************************************************************************
*				Programme de conversion de fichier MPS (IC-CAP model) 				 *
*                    vers le format SCS (model Cadence)                              *
**************************************************************************************
																by Laurent VARIZAT

	INFOS :
		1) Ce programme prend trois arguments :
                - le type de transistor : p pour Pmos et n pour Nmos
				- le fichier MPS à lire
				- le nom du fichier SCS destination

																					 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

using namespace std;


int main (int argc, char * argv[])
{
    map<string, double> params;
    int                 cpt_line = 0;

    cout << "Execution du programme de conversion de fichier MDM vers CSV ...\n" << endl;

    // Récupération du nom de fichier
    if (argc != 4) {
        cout << "Veuillez entrer 3 arguments : \
               \n - le type de transistor : n pour NMOS et p pour PMOS \
               \n - le nom du fichier MPS de départ \
               \n - le nom du fichier SCS destination. \n" << endl;
        return 0;
    }

    string type = argv[1];
    double calc1, calc2;
    if (!type.compare("p")) {
        calc1 = 1.03e-08;
        calc2 = 7.1e-9;
    }
    else {
        calc1 = 0.67e-08;
        calc2 = 4.9e-9;
    }

    ifstream fmps(argv[2]);
    if (!fmps) {
        cout << "Erreur lors de l'ouverture du fichier " << argv[2] << endl;
        return 0;
    }

    ofstream fscs(argv[3]);
    if (!fscs) {
        cout << "Erreur lors de la création du fichier " << argv[3] << endl;
        return 0;
    }

    // On parcours le fichier MPS tant qu'on n'a pas d'erreur
    stringstream    line;
    string          lineRaw, cle, supp;
    char            sup;
    double          val;

    while (getline(fmps, lineRaw)) {
            // On récupère chacune des variables qu'on stock dans une table de hachage
            if ((++cpt_line > 7) && (cpt_line < 171)) {
                line << lineRaw;
                line >> supp >> cle >> sup >> val;
                // debug : cout << "cle = " << cle << " & val = " << val << endl;
                line.clear();

                params[cle] = val;  // On le stock dans une table de hashage
            }
    }

    // On écrit dans le nouveau fichier SCS
        // Préambule :
    fscs << " //---------------------------------------------------        "                << endl;
    fscs                                                                                    << endl;
    fscs << " // SPECTRE DIRECT                                           "                 << endl;
    fscs << " // MOS transistor library file  "                                             << endl;
    fscs << " //---------------------------------------------------       "                 << endl;
    fscs << " library cmos                                                "                 << endl;
    fscs << " section cmostm                                              "                 << endl;
    fscs << "                                                                            "  << endl;
    fscs << " // -----------------------------------------------------    "                 << endl;
    fscs << " // Owner: L2E                                               "                 << endl;
    fscs << " // HIT-Kit: Digital                                         "                 << endl;
    fscs << " // ********** SIMULATION PARAMETERS ********************    "                 << endl;
    fscs << " //  ----------------------------------------------------    "                 << endl;
    fscs << " //  format    : Spectre (Spectre Direct)   "                                  << endl;
    fscs << " //  model     : MOS BSIM3v3   "                                               << endl;
    fscs << " //  process   : C35   "                                                       << endl;
    fscs << " //  revision  : 4.0;   "                                                      << endl;
    fscs << " //  extracted : B10866 ; 2015-04; ese(5487)   "                               << endl;
    fscs << " //  doc#      : ENG-182 REV_6   "                                             << endl;
    fscs << " //  ----------------------------------------------------------------------  " << endl;
    fscs << " //                          TYPICAL MEAN CONDITION   "                        << endl;
    fscs << " //     "                                                                      << endl;
    fscs << " //  ----------------------------------------------------------------------  " << endl;
    fscs << " //   "                                                                        << endl;
    fscs << " inline subckt mod" << type << " ( d g s b )   "                               << endl;
    fscs << " parameters w=1.0e-6 l=1.0e-6 nrd=0.0 nrs=0.0 ad=0.0 as=0.0 pd=0.0 ps=0.0 ng=1   " << endl;
    fscs << " + mmvth=0.0 mmu0=0.0   " << endl;
    fscs << " + dvthmat=(mv_mod" << type << "+mmvth)*(" << calc1 << "/sqrt(l*w*ng)) dumat=(mu_mod" << type << "+mmu0)*(" << calc2 << "/sqrt(l*w*ng))   " << endl;
    fscs << " soac_m1 ( d g s b psub! ) mod" << type << "_ahdl   " << endl;
    fscs << " if (mod" << type << "==1)   " << endl;
    fscs << " mod" << type << " ( d g s b ) mosinsub w=w l=l nrd=nrd nrs=nrs ad=ad as=as pd=pd ps=ps m=ng   " << endl;
    fscs << " else   " << endl;
    fscs << " mod" << type << " ( d s ) resistor r=1e30   " << endl;
    fscs << " notvalid paramtest errorif=(mod" << type << "==0) \\   " << endl;
    fscs << " message=\"   " << endl;
    fscs << " ***************************************************   " << endl;
    fscs << " *   " << endl;
    fscs << " * " << type << "mos is not valid in selected process option.   " << endl;
    fscs << " *   " << endl;
    fscs << " ***************************************************\"   " << endl;
    fscs << " model mosinsub bsim3v3 version=" << params["VERSION"] << " type=" << type << "  capmod=" << params["CAPMOD"] << " \\   " << endl;
    fscs << " mobmod=" << params["MOBMOD"] << "  nqsmod=" << params["NQSMOD"] << "  noimod=" << params["NOIMOD"] << " \\   " << endl;
    fscs << " vbox=1.000e+03      k1=" << params["K1"] << " \\   " << endl;
    fscs << " k2=" << params["K2"] << "      k3=" << params["K3"] << "     k3b=" << params["K3B"] << " \\   " << endl;
    fscs << " nch=" << params["NCH"] << " vth0=" << params["VTH0"] << " \\   " << endl;
    fscs << " voff=" << params["VOFF"] << "    dvt0=" << params["DVT0"] << "    dvt1=" << params["DVT1"] << " \\   " << endl;
    fscs << " dvt2=" << params["DVT2"] << "    keta=" << params["KETA"] << " \\   " << endl;
    fscs << " pscbe1=" << params["PSCBE1"] << "  pscbe2=" << params["PSCBE2"] << " \\   " << endl;
    fscs << " alpha0=" << params["ALPHA0"] << "  alpha1=" << params["ALPHA1"] << "   beta0=" << params["BETA0"] << " \\   " << endl;
    fscs << " dvt0w=" << params["DVT0W"] << "   dvt1w=" << params["DVT1W"] << "   dvt2w=" << params["DVT2W"] << " \\   " << endl;
    fscs << " ua=" << params["UA"] << "      ub=" << params["UB"] << "      uc=" << params["UC"] << " \\   " << endl;
    fscs << " u0=" << params["U0"] << " \\   " << endl;
    fscs << " dsub=" << params["DSUB"] << "    eta0=" << params["ETA0"] << "    etab=" << params["ETAB"] << " \\   " << endl;
    fscs << " nfactor=" << params["NFACTOR"] << " \\   " << endl;
    fscs << " em=" << params["EM"] << "    pclm=" << params["PCLM"] << " \\   " << endl;
    fscs << " drout=" << params["DROUT"] << " \\   " << endl;
    fscs << " a0=" << params["A0"] << "      a1=" << params["A1"] << "      a2=" << params["A2"] << " \\   " << endl;
    fscs << " pvag=" << params["PVAG"] << "    vsat=" << params["VSAT"] << "     ags=" << params["AGS"] << "      b0=" << params["B0"] << "      b1=" << params["B1"] << " \\   " << endl;
    fscs << " delta=" << params["DELTA"] << " pdiblcb=" << params["PDIBLCB"] << " \\   " << endl;
    fscs << " pdiblc1=" << params["PDIBLC1"] << " pdiblc2=" << params["PDIBLC2"] << " \\   " << endl;
    fscs << " w0=" << params["W0"] << " \\   " << endl;
    fscs << " dlc=" << params["DLC"] << " \\   " << endl;
    fscs << " dwc=" << params["DWC"] << "     dwb=" << params["DWB"] << "     dwg=" << params["DWG"] << " \\   " << endl;
    fscs << " ll=" << params["LL"] << "      lw=" << params["LW"] << "     lwl=" << params["LWL"] << " \\   " << endl;
    fscs << " lln=" << params["LLN"] << "     lwn=" << params["LWN"] << "      wl=" << params["WL"] << " \\   " << endl;
    fscs << " ww=" << params["WW"] << "     wwl=" << params["WWL"] << "     wln=" << params["WLN"] << " \\   " << endl;
    fscs << " wwn=" << params["WWN"] << " \\   " << endl;
    fscs << " at=" << params["AT"] << "     ute=" << params["UTE"] << " \\   " << endl;
    fscs << " kt1=" << params["KT1"] << "     kt2=" << params["KT2"] << "    kt1l=" << params["KT1L"] << " \\   " << endl;
    fscs << " ua1=" << params["UA1"] << "     ub1=" << params["UB1"] << "     uc1=" << params["UC1"] << " \\   " << endl;
    fscs << " prt=" << params["PRT"] << " \\   " << endl;
    fscs << " cgso=" << params["CGSO"] << "    cgdo=" << params["CGDO"] << "    cgbo=" << params["CGBO"] << " \\   " << endl;
    fscs << " cgdl=" << params["CGDL"] << "    cgsl=" << params["CGSL"] << "  ckappa=" << params["CKAPPA"] << " \\   " << endl;
    fscs << " cf=" << params["CF"] << "     elm=" << params["ELM"] << " \\   " << endl;
    fscs << " xpart=" << params["XPART"] << "     clc=" << params["CLC"] << "     cle=" << params["CLE"] << " \\   " << endl;
    fscs << " noff=" << params["NOFF"] << "  voffcv=" << params["VOFFCV"] << " \\   " << endl;
    fscs << " rdsw=" << params["RDSW"] << " \\   " << endl;
    fscs << " cdsc=" << params["CDSC"] << "   cdscb=" << params["CDSCB"] << "   cdscd=" << params["CDSCD"] << " \\   " << endl;
    fscs << " prwb=" << params["PRWB"] << "    prwg=" << params["PRWG"] << "     cit=" << params["CIT"] << " \\   " << endl;
    fscs << " tox=" << params["TOX"] << " \\   " << endl;
    fscs << " ngate=" << params["NGATE"] << " \\   " << endl;
    fscs << " nlx=" << params["NLX"] << " \\   " << endl;
    fscs << " xl=" << params["XL"] << "      xw=" << params["XW"] << " \\   " << endl;
    fscs << " af=" << params["AF"] << "      kf=" << params["KF"] << "      ef=" << params["EF"] << " \\   " << endl;
    fscs << " noia=" << params["NOIA"] << "    noib=" << params["NOIB"] << "    noic=" << params["NOIC"] << " \\   " << endl;
    fscs << " rd=" << params["RD"] << "      rs=" << params["RS"] << "     rsh=" << params["RSH"] << " \\   " << endl;
    fscs << " minr=1.000e-03 \\   " << endl;
    fscs << " rdc=" << params["RDC"] << "     rsc=" << params["RSC"] << " lint=" << params["LINT"] << " \\   " << endl;
    fscs << " wint=" << params["WINT"] << "   ldif=" << params["LDIF"] << "    hdif=" << params["HDIF"] << " \\   " << endl;
    fscs << " xj=" << params["XJ"] << "      js=" << params["JS"] << " \\   " << endl;
    fscs << " n=1.0000e+00 \\   " << endl;
    fscs << " dskip=no          tlev=0           tlevc=0         \\   " << endl;
    fscs << " jsw=" << params["JSW"] << " \\   " << endl;
    fscs << " cj=" << params["CJ"] << "    cjsw=" << params["CJSW"] << " \\   " << endl;
    fscs << " fc=0.0000e+00    fcsw=0.000e+00 \\   " << endl;
    fscs << " mj=" << params["MJ"] << "    mjsw=" << params["MJSW"] << " \\   " << endl;
    fscs << " pb=" << params["PB"] << "    pbsw=" << params["PBSW"] << "     xti=" << params["XTI"] << endl;
    fscs << " ends mod" << type << "  " << endl;

	cout << "SUCCESS" << endl << endl;

    return 0;
}
