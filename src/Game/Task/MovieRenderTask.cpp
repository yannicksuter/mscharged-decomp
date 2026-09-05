#include "Game/Task/MovieRenderTask.h"

#include "Game/Sys/movie.h"

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
