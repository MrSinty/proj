#include "labyrinth.h"


Labyrinth::Labyrinth(int width, int height, bool random, int start_x, int start_y, int finish_x, int finish_y)
{
	this->width = width;
	this->height = height;
	labyrinth = new node* [width];
	for (int i = 0; i < width; i++)
	{
		labyrinth[i] = new node[height];
	}

	srand(time(NULL));
    int startX, startY, finishX, finishY;
    if (random == true){
        startX = rand() % (width - 2) + 1;
        startY = rand() % (height - 2) + 1;
        finishX = rand() % (width - 2) + 1;
        finishY = rand() % (height - 2) + 1;
    }
    else{
        startX = start_x;
        startY = start_y;
        finishX = finish_x;
        finishY = finish_y;
    }

	for (int y = 0; y < height; y++)
	{
		
		if (y == 0 || y == height - 1)
		{
			for (int x = 0; x < width; x++)
			{
				labyrinth[x][y].x = x;
				labyrinth[x][y].y = y;
				labyrinth[x][y].typeOfCell = WALL_SYMBOL;
			}
		}
		else
		{
			for (int x = 0; x < width; x++)
			{
				labyrinth[x][y].x = x;
				labyrinth[x][y].y = y;
				if (x == 0 || x == width - 1)
					labyrinth[x][y].typeOfCell = WALL_SYMBOL;
				else if (x == startX && y == startY){
					labyrinth[x][y].typeOfCell = START_SYMBOL;
					start.x = x;
					start.y = y;
					start.typeOfCell = START_SYMBOL;
				}
				else if (x == finishX && y == finishY){
					labyrinth[x][y].typeOfCell = FINISH_SYMBOL;
					goal.x = x;
					goal.y = y;
					goal.typeOfCell = FINISH_SYMBOL;
				}
				else
                    labyrinth[x][y].typeOfCell = rand() % 4;
			}
		}
	}
}


node Labyrinth::getStartNode()
{
	return start;
}

node Labyrinth::getGoalNode()
{
	return goal;
}

bool Labyrinth::findWay()
{
	find_path();
	return status;
}

void Labyrinth::drawWay()
{
	draw_path();
}

void Labyrinth::printLabyrinth()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			std::cout << labyrinth[x][y].typeOfCell << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Labyrinth::printDebug()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			std::cout << labyrinth[x][y];
		}
		std::cout << std::endl << std::endl;
	}
    std::cout << std::endl;
}

node** Labyrinth::getLabyrinth()
{
    return labyrinth;
}

void Labyrinth::calculateHCostForNode(int x, int y)
{
	labyrinth[x][y].calculateHCost(goal);
}

void Labyrinth::calculateFCostForNode(int x, int y)
{
	labyrinth[x][y].calculateFCost();
}

void Labyrinth::find_path()
{
    QElapsedTimer timer;
    timer.start();

	std::vector<node> unmarkedNodes;
	std::vector<node> markedNodes;
	labyrinth[start.x][start.y].gCost = 0;
	calculateHCostForNode(start.x, start.y);
	calculateFCostForNode(start.x, start.y);
	unmarkedNodes.push_back(labyrinth[start.x][start.y]);
	while (unmarkedNodes.size() != 0)
	{
		node current = lowestFCostNode(unmarkedNodes);
		if (current.x == goal.x && current.y == goal.y)
		{
			goal = labyrinth[goal.x][goal.y];
			start = labyrinth[start.x][start.y];
			status = true;
			return;
		}
		unmarkedNodes.erase(find(unmarkedNodes.begin(), unmarkedNodes.end(), current));
		markedNodes.push_back(current);

		for (int y = current.y + 1; y > current.y - 2; y--)
		{
			for (int x = current.x - 1; x < current.x + 2; x++)
			{
				if ((x == current.x && y == current.y) || y >= height || x >= width || x < 0 || y < 0 || labyrinth[x][y].typeOfCell == WALL_SYMBOL) continue;

                int wayCost = labyrinth[x][y].gCost + calculateNeighboursCosts(labyrinth[x][y], current, ROAD_WEIGHT, DIRTY_ROAD_WEIGHT, LAKE_WEIGHT);

				if (find(markedNodes.begin(), markedNodes.end(), labyrinth[x][y]) != markedNodes.end() || wayCost >= labyrinth[x][y].gCost && labyrinth[x][y].gCost != -1) continue;
				labyrinth[x][y].parentX = current.x;
				labyrinth[x][y].parentY = current.y;
				labyrinth[x][y].gCost = wayCost;
				labyrinth[x][y].calculateHCost(goal);
				labyrinth[x][y].calculateFCost();
				if (find(unmarkedNodes.begin(), unmarkedNodes.end(), labyrinth[x][y]) == unmarkedNodes.end()) unmarkedNodes.push_back(labyrinth[x][y]);
			}
		}
	}
	status = false;
    timeEl = timer.elapsed();
}

void Labyrinth::draw_path()
{
    QFile file("../Write.xml");
    if(!file.open(QIODevice::WriteOnly))
        qDebug() << "Fail";

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("info");
    xmlWriter.writeStartElement("path");

    xmlWriter.writeStartElement("start");
    QString coords = QString::number(start.x) + " " + QString::number(start.y);
    xmlWriter.writeCharacters(coords);
    xmlWriter.writeEndElement();

	node currentNode = goal;
    int len = 0;
	while (currentNode != start)
	{
        ++len;
		if (currentNode != goal && currentNode != start)
		{
            if (currentNode.typeOfCell == DIRTY_CELL_SYMBOL)
				labyrinth[currentNode.x][currentNode.y].typeOfCell = DIRTY_PATH_SYMBOL;
            else if (currentNode.typeOfCell == LAKE_SYMBOL)
            {
                labyrinth[currentNode.x][currentNode.y].typeOfCell = LAKE_PATH_SYMBOL;
            }
			else
			{
				labyrinth[currentNode.x][currentNode.y].typeOfCell = PATH_SYMBOL;
			}
		}
		else if (currentNode == start) return;

        if (currentNode.parentX != start.x && currentNode.parentX != start.y){
            xmlWriter.writeStartElement("cell");
            QString coords = QString::number(currentNode.parentX) + " " + QString::number(currentNode.parentY);
            xmlWriter.writeCharacters(coords);
            xmlWriter.writeEndElement();
        }
		currentNode = labyrinth[currentNode.parentX][currentNode.parentY];
	}
    ++len;

    xmlWriter.writeStartElement("end");
    coords = QString::number(goal.x) + " " + QString::number(goal.y);
    xmlWriter.writeCharacters(coords);
    xmlWriter.writeEndElement();

    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("length");
    xmlWriter.writeCharacters(QString::number(len));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("time");
    xmlWriter.writeCharacters(QString::number(timeEl));
    xmlWriter.writeEndElement();

    xmlWriter.writeEndElement();
    file.close();
}

void Labyrinth::setWeight(int rW, int drW, int lW)
{
    ROAD_WEIGHT = rW;
    DIRTY_ROAD_WEIGHT = drW;
    LAKE_WEIGHT = lW;
}
