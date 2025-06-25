#include <vector>
#include <iostream>
#include <utility>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <map>
#include <algorithm>

using namespace std;

string pad_minus(const string& s) {
    string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '-' &&
            i > 0 && i + 1 < s.size() &&
            isalnum(static_cast<unsigned char>(s[i - 1])) &&
            isalnum(static_cast<unsigned char>(s[i + 1]))) {
            out += " - ";          // 兩側都是字母/數字 → 補空白
        } else {
            out += s[i];
        }
    }
    return out;
}

struct Action{
	vector<string> sstate;
	vector<string> cond;
	vector<string> dstate;
	vector<string> state;

};

struct ECCstate{
	string sname;
	vector<string> cond;
	vector<string> dstate;
	vector<Action> act;
};

struct EO{
	string eoname;
	vector<string> eostate;
	vector<string> eocond;
};

struct Vint{
	string var;
	vector<string> sstate;
	vector<string> cond;
	vector<string> nxt_val;
};

struct Indata{
	string var;
	vector<string> sstate;
	vector<string> cond;
	vector<string> nxt_val;
};

struct Outdata{
	string var;
	vector<string> sstate;
	vector<string> cond;
	vector<string> nxt_val;
};

struct FB{
	string name;
	
	string t1cond;
	string t4cond;

	vector<string> ei;
	vector<EO> eo;
	
	vector<Indata> vi;
	vector<string> vi_assign;
	
	vector<Outdata> vo;
	vector<string> vo_assign;
	
	vector<Vint> vint; 
	vector<string> vint_assign;
	
	
	
	vector<string>osm_state;
	
	vector<ECCstate> ecs;	
	
	vector<string> t2s;
	vector<string> t2c;
	
	vector<string> t3s;
	vector<string> t3c;
};





int main(){
	ifstream fin("traffic_light.txt");
	
	ofstream fout("traffic_light.smv");
	
	string word;
	
	vector<FB> fb_vec;
	
	while(!fin.eof()){
		
		fin >> word;
						
		if(word == "FunctionBlock"){//start read function block code
			FB fb;
			
			fin >> word;
			
			fb.name = word; 			//function block name
			
			//cout << fb.name << endl;
			
			fin >> word;
			
			if(word == "begin"){

				while(word != "end"){
					fin >> word;
					//cout << word <<endl;
					if(word == "input_event"){
						fin >> word;
						while(word !=";"){
							//cout << word << endl;
							fb.ei.push_back(word);
							fin >> word;
						}
					}
					else if(word == "output_event"){
						fin >> word;
						while(word !=";"){
							//cout << word << endl;
							
							EO eout;
							eout.eoname=word;
							
							fb.eo.push_back(eout);
							fin >> word;
						}
					}
					else if(word == "input_data"){
						fin >> word;
						while(word!=";"){	
								//cout <<"----------"<< word<<"----------";
							string str;
							str=word;
							int index=str.find(":");
							
							Indata v;
							
							str = str.assign(str,0,index);
							//cout <<"----------"<< word<<"----------"<<endl;
							fb.vi_assign.push_back(word);
							//cout <<"----------"<< str<<"----------"<<endl;
							v.var=str;
							fb.vi.push_back(v);
							fin >> word;
						}
						
					}
					else if(word == "output_data"){
						fin >> word;
						while(word!=";"){	
								//cout <<"----------"<< word<<"----------";
							string str;
							str=word;
							int index=str.find(":");
							
							Outdata v;
							
							str = str.assign(str,0,index);
							//cout <<"----------"<< word<<"----------"<<endl;
							fb.vo_assign.push_back(word);
							//cout <<"----------"<< str<<"----------"<<endl;
							v.var=str;
							fb.vo.push_back(v);
							fin >> word;
						}
					}
					else if(word == "internal_variable"){
						fin >> word;
						while(word!=";"){	
								//cout <<"----------"<< word<<"----------";
							string str;
							str=word;
							int index=str.find(":");
							
							Vint v;
							
							str = str.assign(str,0,index);
							//cout <<"----------"<< word<<"----------"<<endl;
							fb.vint_assign.push_back(word);
							//cout <<"----------"<< str<<"----------"<<endl;
							v.var=str;
							fb.vint.push_back(v);
							fin >> word;
						}
					}
					else if(word == "ECC"){
						fin >> word;
						if(word == "ebegin"){
							
							while(word != "eend"){
								fin >> word;
								if(word == "state"){
									fin >> word;
									while(word != ";"){
										ECCstate eccs;
										eccs.sname = word;
										
										fb.ecs.push_back(eccs);
										
										//cout << eccs.sname << endl;
										
										fin>>word;
									}
									
									
								}
								else if(word == "transition"){
									fin >> word;
									if(word == "("){
										fin >> word;	
										while(word != ")"){
											for(int i=0; i<fb.ecs.size(); i++){
												if(fb.ecs[i].sname == word){
													string str;
													fin >> str;
													fb.ecs[i].cond.push_back(str);
													//cout << str << endl;
													fin >> str;
													fb.ecs[i].dstate.push_back(str);
													//cout << str << endl;
												}
											}
											fin >> word;
										}
									}
								}
								else if(word == "action"){
									fin >> word;
									if(word == "("){
										fin >> word;	
										while(word != ")"){// follow the ECC state
											//cout <<"State="<< word <<endl;
											string str1=word;
											//cout <<"****"<< str1 <<endl;
											int nstate;
											for(int i=0; i<fb.ecs.size(); i++)
												if(word == fb.ecs[i].sname)
													nstate=i;
											
											
											
											
											fin >> word;
											if(word == "{"){
												fin>>word;
												while(word != "}"){//sub-action
													//cout <<"Action="<< word <<endl;
													
													bool any_eo=false;
													
													string str2=str1;
													str2=str2+word;	
													//cout << "-------" << str2 <<endl;												
													fin >> word;
													Action a;
													int eoindex;

													if(word == "["){
														fin >> word;

														while(word != "]"){// sub-action FSM
															
															if(word == "state"){
																fin >> word;
																while(word != ";"){
																	string str4=str2;
																	str4=str4+word;
																	a.state.push_back(str4);
																	//cout <<"Action state="<< str4<<endl;
																	fin >> word;
																}
																
															}
															
															else if(word == "transition"){
																fin >> word;
																if(word == "("){
																	fin >> word;	
																	while(word != ")"){
																		while(word != ";"){
																			
																			string str3=str2;
																			string str4;
																			str3=str3+word;//transition source state
																			//cout <<"transition = "<< str3 <<"--";
																			
																			a.sstate.push_back(str3);
																			
																			fin >> word;// transition condition
																			//cout << word << "--";
																			str4=word;
																			a.cond.push_back(word);
																			fin >> word;
																			if(word == "/"){
																				fin >> word;
																				while(word !="|"){//transition action
																					//cout << word << "->"; 
																					int index=word.find("=");
																					
																					string vint;
																					vint=vint.assign(word,0,index);
																					
																					string var;
																					var=var.assign(word,index+1,word.size());
																					
																					//cout << "***********************"<< vint << "***************"<< var <<endl;
																					
																					for(int j=0; j<fb.vint.size(); j++){
																						if(fb.vint[j].var==vint){
																							fb.vint[j].sstate.push_back(str3);
																							fb.vint[j].cond.push_back(str4);
																							fb.vint[j].nxt_val.push_back(var);
																							cout << "****Vint***"<< str3 << "*************"<< str4 << "*********" << var <<endl;
																						}
																					}
																					for(int j=0; j<fb.vo.size(); j++){
																						if(fb.vo[j].var==vint){
																							fb.vo[j].sstate.push_back(str3);
																							fb.vo[j].cond.push_back(str4);
																							fb.vo[j].nxt_val.push_back(var);
																							cout << "****VOUT***"<< str3 << "*************"<< str4 << "*********" << var <<endl;
																						}
																					}
																					
																					fin >> word;
																				}
																				fin >> word;// transition destination state
																				str3=str2+word;
																				a.dstate.push_back(str3);
																				//cout << str3 <<endl;
																				fin >> word;
																			}
																			else{
																				str3=str2+word;
																				a.dstate.push_back(str3);
																				//cout << str3 <<endl;
																				fin >> word;
																			}
																			
																		}
																		fin >> word;
																	}
																}	
															}
															
															else if(word == "output_event"){
																fin >> word;
																any_eo=true;
																while(word != ";"){
																	//cout << "output event=" << word <<endl;
																	for(int j=0; j< fb.eo.size(); j++){
																		if(fb.eo[j].eoname==word){																			
																			eoindex=j;
																			break;
																		}
																	}
																	fin >> word;
																}
																
															}
															fin >> word;
															
														}
														
													}
													if(any_eo){
														for(int j=0; j< a.dstate.size(); j++){
															int col=a.state.size()-1;
															if(a.dstate[j]==a.state[col]){
																fb.eo[eoindex].eocond.push_back(a.cond[j]);
																//cout << "------------------------" << a.cond[j]<<endl;
																fb.eo[eoindex].eostate.push_back(a.sstate[j]);
																//cout << "------------------------" << a.sstate[j]<<endl;
															}
														
														}
													}

													fb.ecs[nstate].act.push_back(a);
													fin >> word;
												}
											}
											
											fin >> word;
										}
									}
								}
							}
						}
					}
					else if(word == "OSM"){
						fin >> word;
						
						if(word == "obegin"){
							while(word!= "oend"){
								fin >> word;

								if(word == "state"){
									fin >> word;
									while(word != ";"){
										
										fb.osm_state.push_back(word);
										cout << word <<endl;
										fin >> word;
									}
								}
								else if(word == "transition"){
									fin >> word;
									
									if(word=="("){
										while(word != ")"){
											fin >> word;	
											if(word == "t1"){
												fin >> word;
												fb.t1cond=word;
												fin >>word;
											}
											else if(word == "t4"){
												fin >> word;
												fb.t4cond=word;
												fin >>word; 
											}
											else if(word == "t2"){
												fin >> word;
												if(word == "["){
													fin >>word;
													while(word != "]"){
														
														cout << word<<"---";
														fb.t2s.push_back(word);
														fin >> word;
														cout << word <<endl;
														fb.t2c.push_back(word);
														fin >>word;
													}
													
													
												}
												
												
											}
											else if(word == "t3"){
												fin >> word;
												if(word == "["){
													fin >>word;
													while(word != "]"){
														
														cout << word<<"---";
														fb.t3s.push_back(word);
														fin >> word;
														cout << word <<endl;
														fb.t3c.push_back(word);
														fin >>word;
													}
													
													
												}
											}
											
										}
									}
									
									
								}
								
								
							}				
						}
						
						
					}
					
				}
			}
			
			fb_vec.push_back(fb);
		}//end build fb
	}// end read file
	
	for(int i=0; i<fb_vec.size(); i++){
		string t1cond="(";
		fout << "MODULE " <<fb_vec[i].name<<"(";
		for(int j=0; j<fb_vec[i].ei.size(); j++){
			fout << fb_vec[i].ei[j] << "_";
			t1cond=t1cond+fb_vec[i].ei[j]+"_";
			if(j<fb_vec[i].ei.size()-1){
				fout << ", ";
				t1cond=t1cond+" | ";
			}
		}
		
		if(!fb_vec[i].vi.empty()){
			fout << ", ";
		}
			
		t1cond=t1cond+")";
		
		for(int j=0; j<fb_vec[i].vi.size(); j++){
			fout << fb_vec[i].vi[j].var << "_";

			if(j<fb_vec[i].vi.size()-1){
				fout << ", ";

			}
		}


		
		fout << ")" << endl;  //MODULE define
		
		fout << "VAR" << endl;

		for(int j=0; j<fb_vec[i].ei.size(); j++){
			fout  << "	"<< fb_vec[i].ei[j] << ": boolean;" << endl;
		}
		fout << endl;

		for(int j=0; j<fb_vec[i].vi_assign.size(); j++)
			fout  << "	"<< fb_vec[i].vi_assign[j]<<";"<<endl;
		fout << endl;
		
		for(int j=0; j<fb_vec[i].vo_assign.size(); j++)
			fout  << "	"<< fb_vec[i].vo_assign[j]<<";"<<endl;
		fout << endl;
			
		for(int j=0; j<fb_vec[i].vint_assign.size(); j++)
			fout  << "	"<< fb_vec[i].vint_assign[j]<<";"<<endl;
		
		fout << endl;
		fout << "	";
		fout << "State:{";
		for(int j=0; j<fb_vec[i].ecs.size(); j++){				//State VAR
			fout << fb_vec[i].ecs[j].sname << "s0, ";
			fout << fb_vec[i].ecs[j].sname << "s1, ";
			fout << fb_vec[i].ecs[j].sname << "s2, ";
			
			if(!fb_vec[i].ecs[j].act.empty()){
				for(int k=0; k<fb_vec[i].ecs[j].act.size(); k++){
					for(int l=0; l<fb_vec[i].ecs[j].act[k].state.size(); l++){
						fout << fb_vec[i].ecs[j].act[k].state[l];
						bool done=false;
						if((j==(fb_vec[i].ecs.size()-1)) && (k==(fb_vec[i].ecs[j].act.size()-1)) && (l==(fb_vec[i].ecs[j].act[k].state.size()-1)))
							done=true;
						
							
						if(!done)
							fout << ", ";
					}
				}
			}
			else{ 
				fout << fb_vec[i].ecs[j].sname << "exdone";
				if(j!=(fb_vec[i].ecs.size()-1))
					fout << ", ";
			} 

		}
		fout << "};" << endl<<endl;

		for(int j=0; j<fb_vec[i].eo.size(); j++) //output event VAR assignment
			fout  << "	"<< fb_vec[i].eo[j].eoname<< ": boolean;"<<endl;
		
		fout << endl;

		for(int j=0; j<fb_vec[i].ecs.size(); j++)//action control signal VAR assignment
			fout  << "	"<< fb_vec[i].ecs[j].sname <<"Act: boolean;"<<endl;
		fout <<endl;
		
		fout << "ASSIGN" <<endl;
		for (int j = 0; j < fb_vec[i].vint.size(); ++j) {
			fout << "	init(" << fb_vec[i].vint[j].var << "):=0;" << endl;
		}
		for (int j = 0; j < fb_vec[i].ei.size(); j++){
			fout << "    init(" << fb_vec[i].ei[j] << ") := FALSE;" << endl;
		}
		for (int j = 0; j < fb_vec[i].eo.size(); j++){
			fout << "    init(" << fb_vec[i].eo[j].eoname << ") := FALSE;" << endl;
		}

		fout << "	init(State):=q0s0;" <<endl;
		fout << "	next(State):=case" <<endl;
		for(int j=0; j<fb_vec[i].ecs.size(); j++){					//fout State assignment
			fout << "		State="<< fb_vec[i].ecs[j].sname<< "s0 & " << t1cond << ": "<< fb_vec[i].ecs[j].sname<< "s1;" <<endl; //t1 transition
			
			for(int k=0; k<fb_vec[i].ecs[j].cond.size(); k++){
				fout << "		State="<< fb_vec[i].ecs[j].sname<< "s1 & (";
				fout << pad_minus(fb_vec[i].ecs[j].cond[k]) <<"): "<< fb_vec[i].ecs[j].dstate[k] << "s2;"<< endl;//t3 transition
			}
			
			fout << "		State="<< fb_vec[i].ecs[j].sname<< "s1 & ";
			for(int k=0; k<fb_vec[i].ecs[j].cond.size(); k++){
				fout << "!("<< pad_minus(fb_vec[i].ecs[j].cond[k]) <<")";
				if(k<fb_vec[i].ecs[j].cond.size()-1)
					fout << " & ";
			}
			fout << ": "<<fb_vec[i].ecs[j].sname<<"s0;"<<endl;
			
			fout << "		State="<< fb_vec[i].ecs[j].sname<< "s2 & " << fb_vec[i].ecs[j].sname << "Act: "<< fb_vec[i].ecs[j].sname<<"s1;"<<endl;
			
			if(!fb_vec[i].ecs[j].act.empty()){
				string str;
				int nact;
				for(int k=0; k<fb_vec[i].ecs[j].act.size(); k++){
					nact=k;
					for(int l=0; l<fb_vec[i].ecs[j].act[k].sstate.size(); l++){
						if(k==0 && l == 0) // qn->act 1st state
							fout << "		State="<<fb_vec[i].ecs[j].sname << "s2 & !(" << fb_vec[i].ecs[j].sname << "Act): " << fb_vec[i].ecs[j].act[k].state[l]<<";"<<endl;
							
						fout << "		State="<< fb_vec[i].ecs[j].act[k].sstate[l]<< " & (" << pad_minus(fb_vec[i].ecs[j].act[k].cond[l]) << "): " <<fb_vec[i].ecs[j].act[k].dstate[l]<<";"<<endl;
						
						str = fb_vec[i].ecs[j].act[k].dstate[l];//str =sub action final done
					}
					if(k<fb_vec[i].ecs[j].act.size()-1)
						fout << "		State="<< str<<": "<< fb_vec[i].ecs[j].act[k+1].sstate[0]<<";"<<endl;
				}
				fout << "		State="<< str << ": "<<fb_vec[i].ecs[j].sname<<"s2;"<<endl;
				
			}
			else{
				fout << "		State="<< fb_vec[i].ecs[j].sname<<"s2 & !("<< fb_vec[i].ecs[j].sname << "Act): " << fb_vec[i].ecs[j].sname <<"exdone;"<<endl;
				fout << "		State="<< fb_vec[i].ecs[j].sname << "exdone: " <<  fb_vec[i].ecs[j].sname << "s2;"<<endl;
			}
			
		}
		fout << "		TRUE: State;"<<endl;
		fout << "		esac;"<<endl<<endl<<endl;
		
		for(int j=0; j<fb_vec[i].ei.size(); j++){									// fout input event variable
			fout <<"	next("<<fb_vec[i].ei[j]<<"):=case"<<endl;
			fout << "		";
			for(int k=0; k<fb_vec[i].ecs.size(); k++){
				fout <<"(State="<<fb_vec[i].ecs[k].sname<<"s0)";
				if(k<fb_vec[i].ecs.size()-1)
					fout << " | ";				
			}
			fout << " & "<< t1cond;
			fout << ": " << fb_vec[i].ei[j]<<"_;"<<endl;
			
			fout << "		";
			
			for(int k=0; k<fb_vec[i].ecs.size(); k++){
				for(int l=0; l<fb_vec[i].ecs[k].cond.size(); l++){
					fout << "((State=" << fb_vec[i].ecs[k].sname << "s1) & (";
					fout << pad_minus(fb_vec[i].ecs[k].cond[l])<<")) | ";
				}
			}
			
			for(int k=0; k<fb_vec[i].ecs.size(); k++){
				fout << "((State=" << fb_vec[i].ecs[k].sname << "s1) & (";
				for(int l=0; l<fb_vec[i].ecs[k].cond.size(); l++){
					fout << "!(" << pad_minus(fb_vec[i].ecs[k].cond[l]) <<")";
					if(l<fb_vec[i].ecs[k].cond.size()-1)
						fout << " & ";
				}
				fout << "))";
				if(k<fb_vec[i].ecs.size()-1)
					fout << " | ";
			}
			
			fout <<": FALSE;"<< endl;
			fout << "		TRUE: "<<fb_vec[i].ei[j]<<";"<<endl;
			fout << "		esac;"<<endl;
			
		}
		
		for(int j=0; j<fb_vec[i].vi.size(); j++){
			fout <<"	next("<<fb_vec[i].vi[j].var<<"):=case"<<endl;
			fout << "		";
			for(int k=0; k<fb_vec[i].ecs.size(); k++){
				fout <<"(State="<<fb_vec[i].ecs[k].sname<<"s0)";
				if(k<fb_vec[i].ecs.size()-1)
					fout << " | ";				
			}
			fout << " & "<< t1cond;
			fout << ": " << fb_vec[i].vi[j].var<<"_;"<<endl;
			fout << "		TRUE: "<<fb_vec[i].vi[j].var<<";"<<endl;
			fout << "		esac;"<<endl;
		}
			
		for (int j=0; j< fb_vec[i].ecs.size(); j++){//fout action control signal
			
			fout << "	next("<<fb_vec[i].ecs[j].sname << "Act):=case"<<endl;
			
			fout << "		";
			for(int k=0; k<fb_vec[i].ecs.size(); k++){
				for(int l=0; l<fb_vec[i].ecs[k].dstate.size(); l++){
					if(fb_vec[i].ecs[k].dstate[l] == fb_vec[i].ecs[j].sname)
						fout << "(State="<<fb_vec[i].ecs[k].sname<< "s1 & (" << pad_minus(fb_vec[i].ecs[k].cond[l]) <<")) | ";					
				}
			}
			fout << "FALSE: FALSE;" <<endl;
			
			if(!fb_vec[i].ecs[j].act.empty()){
				int final_act_num=fb_vec[i].ecs[j].act.size()-1;
				fout << "		State="<<fb_vec[i].ecs[j].act[final_act_num].state.back() << ": TRUE;" <<endl;
			}
			else{
				fout << "		State="<<fb_vec[i].ecs[j].sname<<"exdone: TRUE;"<<endl;
			}	
			fout << "		TRUE: "<< fb_vec[i].ecs[j].sname<<"Act;"<<endl;
			fout << "		esac;"<<endl;
			
		}
		
		for(int j=0; j<fb_vec[i].vint.size(); j++){
			fout << "	next("<<fb_vec[i].vint[j].var<<"):=case"<<endl;
			
			for(int k=0; k<fb_vec[i].vint[j].sstate.size(); k++)
				fout << "		State="<<fb_vec[i].vint[j].sstate[k]<< " & (" <<pad_minus(fb_vec[i].vint[j].cond[k]) << "): " << pad_minus(fb_vec[i].vint[j].nxt_val[k])<<";"<<endl;
			fout << "		TRUE: "<<fb_vec[i].vint[j].var<<";"<<endl;
			fout <<"		esac;"<<endl;
		}
		
		for(int j=0; j<fb_vec[i].vo.size(); j++){
			fout << "	next("<<fb_vec[i].vo[j].var<<"):=case"<<endl;
			
			for(int k=0; k<fb_vec[i].vo[j].sstate.size(); k++)
				fout << "		State="<<fb_vec[i].vo[j].sstate[k]<< " & (" <<pad_minus(fb_vec[i].vo[j].cond[k]) << "): " << pad_minus(fb_vec[i].vo[j].nxt_val[k])<<";"<<endl;
			fout << "		TRUE: "<<fb_vec[i].vo[j].var<<";"<<endl;
			fout <<"		esac;"<<endl;
		}
		
		for (int j=0; j< fb_vec[i].eo.size(); j++){//fout output event
			fout << "	next("<<fb_vec[i].eo[j].eoname<<"):=case"<<endl;
			fout << "		";
			for(int k=0; k<fb_vec[i].eo[j].eocond.size(); k++){
				fout << "(State="<<fb_vec[i].eo[j].eostate[k]<<" & (" <<pad_minus(fb_vec[i].eo[j].eocond[k])<<")) | ";
				
			}
			fout << "FALSE: TRUE;"<<endl;
			
			fout << "		";
			for(int k=0; k<fb_vec[i].ecs.size(); k++){
				fout << "(State="<< fb_vec[i].ecs[k].sname << "s2 & "<<fb_vec[i].ecs[k].sname << "Act) | ";
				
			}
			fout << "FALSE: FALSE;"<<endl;
			fout << "		TRUE: "<< fb_vec[i].eo[j].eoname<<";"<<endl;
			fout << "		esac;"<<endl;
		}
	}
	
		
	return 0;
}




