/* pattern_search_plugin.cpp
 * finding substructure in a whole neuron with certain morpological pattern.
 * 2017-6-29 : by He Yishan
 */
 
#include "v3d_message.h"
#include <vector>
#include "basic_surf_objs.h"
#include <iostream>
#include "pattern_search_plugin.h"
#include "pattern_analysis.h"
#include "get_subtrees.h"
#include "trees_retrieve.h"

Q_EXPORT_PLUGIN2(pattern_search, pattern_search);

using namespace std;

struct input_PARA
{
    QString inimg_file;
    V3DLONG channel;
    NeuronTree nt_search;
    NeuronTree nt_pattern;
};

void ml_func(V3DPluginCallback2 &callback, QWidget *parent, input_PARA &PARA, bool bmenu);
 
QStringList pattern_search::menulist() const
{
    return QStringList()
		<<tr("tracing_menu")
		<<tr("about");
}

QStringList pattern_search::funclist() const
{
	return QStringList()
		<<tr("tracing_func")
		<<tr("help");
}

void pattern_search::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
	if (menu_name == tr("tracing_menu"))
	{
        bool bmenu = true;
        input_PARA PARA;
        ml_func(callback,parent,PARA,bmenu);

	}
	else
	{
		v3d_msg(tr("finding substructure in a whole neuron with certain morpological pattern.. "
			"Developed by He Yishan, 2017-6-29"));
	}
}

bool pattern_search::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
	if (func_name == tr("tracing_func"))
	{
        bool bmenu = false;
        printf("dofunc can't' work yet.\n");
        return true;
	}
    else if (func_name == tr("help"))
    {
		printf("**** Usage of pattern_search tracing **** \n");
		printf("vaa3d -x pattern_search -f tracing_func -i <inimg_file> -p <channel> <swc_file> <marker_file> <other parameters>\n");
        printf("inimg_file       The input image\n");
        printf("channel          Data channel for tracing. Start from 1 (default 1).\n");
        printf("swc_file         SWC file path.\n");
        printf("marker_file      Marker file path.\n");

        printf("outswc_file      Will be named automatically based on the input image file name, so you don't have to specify it.\n");
        printf("outmarker_file   Will be named automatically based on the input image file name, so you don't have to specify it.\n\n");

	}
	else return false;

	return true;
}


void ml_func(V3DPluginCallback2 &callback, QWidget *parent, input_PARA &PARA, bool bmenu)
{
    if(bmenu)
    {
        PARA.nt_search = readSWC_file("original_vr_neuron.swc");
        PARA.nt_pattern = readSWC_file("areaofinterest.swc");
    }
    else
    {
        printf("In to dofunc.\n");
    }
    cout<< "******************Welcome To Pattern Search*********************"<<endl;

    vector<int> pt_lens;
    vector<NeuronTree> pt_list;
    vector<V3DLONG> result_points;
    if(!pattern_analysis(PARA.nt_search,PARA.nt_pattern,pt_list,pt_lens,callback))
    {
        cout<<"something wrong is in pattern_analysis"<<endl;
        return;
    }
    if(pt_list.size()!=pt_lens.size()) {cout<<"list size is not equal to lens size."<<endl; return;}
    for(int v=0;v<pt_list.size();v++)
    {
        NeuronTree pt = pt_list[v];
        int area_len = pt_lens[v];
        vector<NeuronTree> sub_trees;
        vector<vector<V3DLONG> > p_to_tree;
        get_subtrees(PARA.nt_search,sub_trees,area_len,p_to_tree);
        vector<V3DLONG> selected_trees;
        trees_retrieve(sub_trees,pt,selected_trees);

        cout<<"selected_tree_size="<<selected_trees.size()<<endl;
        cout<<p_to_tree.size()<<endl;

        for(int i=0; i<selected_trees.size();i++)
        {
            V3DLONG id_tree=selected_trees[i];
            vector<V3DLONG> ps_tree=p_to_tree[id_tree];
            for(int j=0;j<ps_tree.size();j++)
            {
                result_points.push_back(ps_tree[j]);
            }
        }
        // for test
//        for(int i=0;i<selected_trees.size();i++)
//        {
//            cout<<selected_trees[i]<<":"<<PARA.nt_search.listNeuron[selected_trees[i]].n<<endl;
//        }
        sub_trees.clear();
        p_to_tree.clear();
        selected_trees.clear();
    }
    //Output
    cout<<"result_points.size="<<result_points.size()<<endl;
    NeuronTree list_search = PARA.nt_search;

    for(V3DLONG i =0; i<result_points.size();i++)
    {
        V3DLONG id=result_points[i];
        list_search.listNeuron[id].type =7;    // type=7 means the color is green
    }
    writeSWC_file("updated_vr_neuron.swc",list_search);
    return;
}










