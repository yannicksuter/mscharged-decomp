#include "Game/Task/MovieRenderTask.h"

#include "Game/Sys/movie.h"

MovieRenderTask::MovieRenderTask()
{
}

void MovieRenderTask::StateTransition(u32, u32)
{
}

void MovieRenderTask::Run(float)
{
    MovieRenderTick();
}
