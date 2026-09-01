#include "Game/Task/MovieRenderTask.h"

extern "C" void fn_80371254();

UnidentifiedMovieRenderTask::UnidentifiedMovieRenderTask()
{
}

void UnidentifiedMovieRenderTask::StateTransition(u32, u32)
{
}

void UnidentifiedMovieRenderTask::Run(float)
{
    fn_80371254();
}
