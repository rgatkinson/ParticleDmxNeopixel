//
// MorseCode.cpp
//
#include "Particle.h"
#include "MorseCode.h"

const MorseCode::Symbol MorseCode::A { Dot,  Dash, };
const MorseCode::Symbol MorseCode::B { Dash, Dot,  Dot,  Dot,  };
const MorseCode::Symbol MorseCode::C { Dash, Dot,  Dash, Dot,  };
const MorseCode::Symbol MorseCode::D { Dash, Dot,  Dot,  };
const MorseCode::Symbol MorseCode::E { Dot,  };
const MorseCode::Symbol MorseCode::F { Dot,  Dot,  Dash, Dot,  };
const MorseCode::Symbol MorseCode::G { Dash, Dash, Dot,  };
const MorseCode::Symbol MorseCode::H { Dot,  Dot,  Dot,  Dot,  };
const MorseCode::Symbol MorseCode::I { Dot,  Dot,  };
const MorseCode::Symbol MorseCode::J { Dot,  Dash, Dash, Dash, };
const MorseCode::Symbol MorseCode::K { Dash, Dot,  Dash, };
const MorseCode::Symbol MorseCode::L { Dot,  Dash, Dot,  Dot,  };
const MorseCode::Symbol MorseCode::M { Dash, Dash, };
const MorseCode::Symbol MorseCode::N { Dash, Dot,  };
const MorseCode::Symbol MorseCode::O { Dash, Dash, Dash, };
const MorseCode::Symbol MorseCode::P { Dot,  Dash, Dash, Dot,  };
const MorseCode::Symbol MorseCode::Q { Dash, Dash, Dot,  Dash, };
const MorseCode::Symbol MorseCode::R { Dot,  Dash, Dot,  };
const MorseCode::Symbol MorseCode::S { Dot,  Dot,  Dot,  };
const MorseCode::Symbol MorseCode::T { Dash, };
const MorseCode::Symbol MorseCode::U { Dot,  Dot,  Dash, };
const MorseCode::Symbol MorseCode::V { Dot,  Dot,  Dot,  Dash, };
const MorseCode::Symbol MorseCode::W { Dot,  Dash, Dash,};
const MorseCode::Symbol MorseCode::X { Dash, Dot,  Dot,  Dash, };
const MorseCode::Symbol MorseCode::Y { Dash, Dot,  Dash, Dash, };
const MorseCode::Symbol MorseCode::Z { Dash, Dash, Dot,  Dot,  };

const MorseCode::Symbol MorseCode::Zero   { Dash, Dash, Dash, Dash, Dash, };
const MorseCode::Symbol MorseCode::One    { Dot,  Dash, Dash, Dash, Dash, };
const MorseCode::Symbol MorseCode::Two    { Dot,  Dot,  Dash, Dash, Dash, };
const MorseCode::Symbol MorseCode::Three  { Dot,  Dot,  Dot,  Dash, Dash, };
const MorseCode::Symbol MorseCode::Four   { Dot,  Dot,  Dot,  Dot,  Dash, };
const MorseCode::Symbol MorseCode::Five   { Dot,  Dot,  Dot,  Dot,  Dot,  };
const MorseCode::Symbol MorseCode::Six    { Dash, Dot,  Dot,  Dot,  Dot,  };
const MorseCode::Symbol MorseCode::Seven  { Dash, Dash, Dot,  Dot,  Dot,  };
const MorseCode::Symbol MorseCode::Eight  { Dash, Dash, Dash, Dot,  Dot,  };
const MorseCode::Symbol MorseCode::Nine   { Dash, Dash, Dash, Dash, Dot,  };

const MorseCode::Symbol* MorseCode::letters[] =
    { &A, &B, &C, &D, &D, &F, &G, &H, &I, &J, &K, &L, &M, &N, &O, &P, &Q, &R, &S, &T, &U, &V, &W, &X, &Y, &Z };

const MorseCode::Symbol* MorseCode::digits[]  =
    { &Zero, &One, &Two, &Three, &Four, &Five, &Six, &Seven, &Eight, &Nine };
