// 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include <lib/pyutil/gil.hpp>
#include <core/GlobalEngine.hpp>
#include <core/Scene.hpp>
#include <pkg/common/PeriodicEngines.hpp>

namespace yade { // Cannot have #include directive inside.

class PyRunner : public PeriodicEngine {
public:
	/* virtual bool isActivated: not overridden, PeriodicEngine handles that */
	virtual void action()
	{
		if (command.size() > 0)
			pyRunString(command, ignoreErrors, updateGlobals);
	}
	// clang-format off
	YADE_CLASS_BASE_DOC_ATTRS(PyRunner,PeriodicEngine,
		"Execute a python command periodically, with defined (and adjustable) periodicity. See :yref:`PeriodicEngine` documentation for details.",
		((string,command,"",,"Command to be run by python interpreter. Not run if empty."))
		((bool,ignoreErrors,false,,"Debug only: set this value to true to tell PyRunner to ignore any errors encountered during command execution."))
		((bool,updateGlobals,true,,"Whether to workaround `ipython not recognizing local variables <https://github.com/ipython/ipython/issues/136>`__ "
		"by calling globals().update(locals()). When true PyRunner is able to call functions declared later locally in a running (live) yade session. "
		"The call is a bit slower to update globals() with recently declared python functions."))
	);
	// clang-format on
};
REGISTER_SERIALIZABLE(PyRunner);

} // namespace yade
