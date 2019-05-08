//
// Created by Paul Motte on 2019-05-06.
//

#ifndef LOX_STATE_H
#define LOX_STATE_H

#include "ptb/ptb_array.h"
#include "ptb/ptb_map.h"

struct		State {
	Array	*errors;

	// Interpreter
	Map		*environment;
};

#endif //LOX_STATE_H
