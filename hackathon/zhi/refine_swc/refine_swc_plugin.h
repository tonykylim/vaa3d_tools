/* refine_swc_plugin.h
 * This is a test plugin, you can use it as a demo.
 * 2018-12-6 : by Zhi Zhou
 */
 
#ifndef __REFINE_SWC_PLUGIN_H__
#define __REFINE_SWC_PLUGIN_H__

#include <QtGui>
#include <v3d_interface.h>

class refine_swc : public QObject, public V3DPluginInterface2_1
{
	Q_OBJECT
	Q_INTERFACES(V3DPluginInterface2_1);

public:
	float getPluginVersion() const {return 1.1f;}

	QStringList menulist() const;
	void domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent);

	QStringList funclist() const ;
	bool dofunc(const QString &func_name, const V3DPluginArgList &input, V3DPluginArgList &output, V3DPluginCallback2 &callback, QWidget *parent);
};

NeuronTree refineSWCTerafly(V3DPluginCallback2 &callback,QString fname_img, NeuronTree nt);
NeuronTree breakSWC(NeuronTree nt, double dist);


#endif

