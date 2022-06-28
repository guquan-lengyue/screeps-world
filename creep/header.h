#ifndef EXAMPLE_CREEPS_HEADER_H
#define EXAMPLE_CREEPS_HEADER_H
#include <vector>
#include <queue>
#include <memory>
class Task;
class Worker;

void initWorkerQueue(std::vector<std::unique_ptr<Worker>> &workers);

std::vector<std::unique_ptr<Worker>> working_queue;
std::priority_queue<std::unique_ptr<Worker>> ready_queue;
std::priority_queue<Task> task_queue;
void takeTask(const Task &task);
void runTask();
int getWorkerNum();
#endif