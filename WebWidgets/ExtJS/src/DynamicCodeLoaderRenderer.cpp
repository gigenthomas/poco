//
// DynamicCodeLoaderRenderer.cpp
//
// $Id: //poco/Main/WebWidgets/ExtJS/src/DynamicCodeLoaderRenderer.cpp#2 $
//
// Library: ExtJS
// Package: Core
// Module:  DynamicCodeLoaderRenderer
//
// Copyright (c) 2007, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/WebWidgets/ExtJS/DynamicCodeLoaderRenderer.h"
#include "Poco/WebWidgets/ExtJS/Utility.h"
#include "Poco/WebWidgets/VerticalLayout.h"
#include "Poco/WebWidgets/DynamicCodeLoader.h"
#include "Poco/WebWidgets/RequestHandler.h"
#include "Poco/NumberFormatter.h"
#include <sstream>


namespace Poco {
namespace WebWidgets {
namespace ExtJS {


DynamicCodeLoaderRenderer::DynamicCodeLoaderRenderer()
{
}


DynamicCodeLoaderRenderer::~DynamicCodeLoaderRenderer()
{
}


void DynamicCodeLoaderRenderer::renderHead(const Renderable* pRenderable, const RenderContext& context, std::ostream& str)
{
	// we need to render two things:
	// - first a load method in the header which loads a js file
	// - second, the js file which must be set as code at the DynamicCodeLoader
	const DynamicCodeLoader* pLoader = static_cast<const DynamicCodeLoader*>(pRenderable);
	poco_assert_dbg (pLoader != 0);
	str <<	"var " << pLoader->loaderFunctionName() << "Loaded = false;" << std::endl;
	str <<	"function " << pLoader->loaderFunctionName() << "(){" << std::endl;
	str <<		"if ("<< pLoader->loaderFunctionName() << "Loaded) return;" << std::endl;
	str <<		pLoader->loaderFunctionName() << "Loaded = true;" << std::endl;
	str <<		"Ext.Ajax.request({" << std::endl;
	str <<			"url: '" << pLoader->uri().toString() << "/;" << RequestHandler::KEY_EVID << "=" << DynamicCodeLoader::EV_LOAD << "&";
	str <<		RequestHandler::KEY_ID << "=" << pLoader->id() << "'," << std::endl;
	str <<			"success: function(response){" << std::endl;
	str <<				"loadScriptDynamically('script" << pLoader->id() << "', response);" << std::endl;
	str <<				"var parent = Ext.getCmp('" << pLoader->parent()->id() << "');" << std::endl;
	str <<				"var child = " << pLoader->functionName() << "();" << std::endl;
	str <<				"parent.add(child);" << std::endl;
	if (!pLoader->getSuccessCall().empty())
		str <<			pLoader->getSuccessCall() << ";" << std::endl;
	str <<			"}";
	if (!pLoader->getErrorCall().empty())
		str <<		",failure:" << pLoader->getErrorCall() << std::endl;
	str <<		"});" << std::endl;
	str <<	"}" << std::endl;
	
	DynamicCodeLoader* pL = const_cast<DynamicCodeLoader*>(pLoader);
	
	WebApplication::instance().registerAjaxProcessor(Poco::NumberFormatter::format(pLoader->id()), pL);
	
	// the js file: only do when not already set
	// bug: this optimization breaks logout/login stuff!
	
	std::ostringstream out;
	View::Ptr pView = pLoader->view();
	out <<	"function " << pLoader->functionName() << "(){";
	out <<		"return ";
				pView->renderHead(context, out);
	out <<		";"; // close return
	out <<	"}";
	
	pL->setViewCode(out.str());
	
	
}


void DynamicCodeLoaderRenderer::renderBody(const Renderable* pRenderable, const RenderContext& context, std::ostream& ostr)
{
}


} } } // namespace Poco::WebWidgets::ExtJS
