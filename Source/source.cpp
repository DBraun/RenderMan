/*
  ==============================================================================

    source.cpp
    Created: 18 Feb 2017 6:37:01pm
    Author:  tollie

  ==============================================================================
*/

#include "PatchGenerator.h"
#include <boost/python.hpp>

// Could also easily be namespace crap.
namespace wrap
{
    //==========================================================================
    // Converts a C++ vector to a Python list. All following functions
    // are essentially cheap ripoffs from this one.
    // https://gist.github.com/octavifs/5362272
    template <class T>
    boost::python::list vectorToList (std::vector<T> vector)
    {
    	typename std::vector<T>::iterator iter;
    	boost::python::list list;
    	for (iter = vector.begin(); iter != vector.end(); ++iter)
        {
    		list.append(*iter);
    	}
    	return list;
    }

    //==========================================================================
    // Yeah this is lazy. I know.
    template <class T>
    boost::python::list arrayToList (std::array<T, 13> array)
    {
    	typename std::array<T, 13>::iterator iter;
    	boost::python::list list;
    	for (iter = array.begin(); iter != array.end(); ++iter)
        {
    		list.append(*iter);
    	}
    	return list;
    }

    // Converts a C++ vector to a python list
    template <class T>
    boost::python::list toPythonList(std::vector<T> vector) {
        typename std::vector<T>::iterator iter;
        boost::python::list list;
        for (iter = vector.begin(); iter != vector.end(); ++iter) {
            list.append(*iter);
        }
        return list;
    }

    //==========================================================================
    // Converts a std::pair which is used as a parameter in C++
    // into a tuple with the respective types int and float for
    // use in Python.
    boost::python::tuple parameterToTuple (std::pair<int, float> parameter)
    {
        boost::python::tuple parameterTuple;
        parameterTuple = boost::python::make_tuple (parameter.first,
                                                    parameter.second);
        return parameterTuple;
    }

    //==========================================================================
    // Converts a PluginPatch ( std::vector <std::pair <int, float>> )
    // to a Python list.
    boost::python::list pluginPatchToListOfTuples (PluginPatch parameters)
    {
    	std::vector<std::pair<int, float>>::iterator iter;
    	boost::python::list list;
    	for (iter = parameters.begin(); iter != parameters.end(); ++iter)
        {
            auto tup = parameterToTuple (*iter);
    		list.append(tup);
    	}
    	return list;
    }

    //==========================================================================
    PluginPatch listOfTuplesToPluginPatch (boost::python::list listOfTuples)
    {
        PluginPatch patch;
        const int size = boost::python::len (listOfTuples);
        patch.reserve (size);
        std::pair <int, float> parameter;
        for (int i = 0; i < size; ++i)
        {
            boost::python::tuple tup;
            tup = boost::python::extract<boost::python::tuple> (listOfTuples[i]);
            int   index = int (boost::python::extract<float> (tup[0]));
            float value = float (boost::python::extract<float> (tup[1]));
            parameter = std::make_pair (index, value);
            patch.push_back (parameter);
        }
        return patch;
    }

    //==========================================================================
    class RenderEngineWrapper : public RenderEngine
    {
    public:

        RenderEngineWrapper (int sr, int bs) :
            RenderEngine (sr, bs)
        { }

		RenderEngineWrapper(const RenderEngineWrapper&) {

		}

        void wrapperSetPatch (boost::python::list listOfTuples)
        {
            PluginPatch patch = listOfTuplesToPluginPatch (listOfTuples);
            RenderEngine::setPatch(patch);
        }

        boost::python::list wrapperGetPatch()
        {
            return pluginPatchToListOfTuples (RenderEngine::getPatch());
        }
        
        float wrapperGetParameter (int parameter)
        {
            return RenderEngine::getParameter(parameter);
        }

        void wrapperSetParameter (int parameter, float value)
        {
            RenderEngine::setParameter(parameter, value);
        }
        
        int wrapperGetPluginParameterSize()
        {
            return int (RenderEngine::getPluginParameterSize());
        }

        boost::python::list wrapperGetAudioFrames()
        {
            std::vector<std::vector<double>> channelBuffers = RenderEngine::getAudioFrames();
            boost::python::list list;
            for (auto oneBuffer = channelBuffers.begin(); oneBuffer != channelBuffers.end(); ++oneBuffer)
            {
                list.append(vectorToList(*oneBuffer));
            }
            return list;
        }

        boost::python::list wrapperGetRMSFrames()
        {
            return vectorToList (RenderEngine::getRMSFrames());
        }
    };
}

//==============================================================================
BOOST_PYTHON_MODULE(librenderman)
{
    using namespace boost::python;
    using namespace wrap;

    class_<RenderEngineWrapper>("RenderEngine", init<int, int>())
    .def("hello", &RenderEngineWrapper::hello)
    .def("n_midi_events", &RenderEngineWrapper::nMidiEvents)
    .def("load_preset", &RenderEngineWrapper::loadPreset)
    .def("load_plugin", &RenderEngineWrapper::loadPlugin)
    .def("load_midi", &RenderEngineWrapper::loadMidi)
    .def("clear_midi", &RenderEngineWrapper::clearMidi)
    .def("add_midi_note", &RenderEngineWrapper::addMidiNote)
    .def("get_patch", &RenderEngineWrapper::wrapperGetPatch)
    .def("set_patch", &RenderEngineWrapper::wrapperSetPatch)
    .def("get_parameter", &RenderEngineWrapper::wrapperGetParameter)
    .def("get_parameter_text", &RenderEngineWrapper::getParameterAsText)
    .def("set_parameter", &RenderEngineWrapper::wrapperSetParameter)
    .def("render", &RenderEngineWrapper::render)
    .def("get_plugin_parameter_size", &RenderEngineWrapper::wrapperGetPluginParameterSize)
    .def("get_plugin_parameters_description", &RenderEngineWrapper::getPluginParametersDescription)
    .def("override_plugin_parameter", &RenderEngineWrapper::overridePluginParameter)
    .def("remove_overriden_plugin_parameter", &RenderEngineWrapper::removeOverridenParameter)
    .def("get_audio_frames", &RenderEngineWrapper::wrapperGetAudioFrames)
    .def("get_rms_frames", &RenderEngineWrapper::wrapperGetRMSFrames)
    .def("write_to_wav", &RenderEngineWrapper::writeToWav);
}
