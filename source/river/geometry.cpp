#include "geometry.hpp"


namespace River{
/*
    Branch object

*/

Branch::Branch(unsigned long int id, Point sourcePoint, double phi):id(id)
{
    sourcePoint.index = id;
    auto points = splitPoint(sourcePoint, phi);
    leftPoints.push_back(points.first);
    rightPoints.push_back(points.second);
}

pair<Point, Point> Branch::splitPoint(Point p, double phi)
{
    Point pLeft{p}, pRight{p}, 
    vecL{-sin(phi),  cos(phi)}, 
    vecR{+sin(phi), -cos(phi)};

    pLeft.index = pRight.index = p.index;
    pLeft  += vecL * eps/2;
    pRight += vecR * eps/2;
    return {pLeft, pRight};
}

Point Branch::mergePoints(Point p1, Point p2)
{
    return (p1 + p2)/2;
}

void Branch::print()
{
    for(unsigned int i = 0; i < leftPoints.size(); ++i )
    {
        cout << "Branch" << endl;
        cout << "size: " << leftPoints.size() << endl;
        cout << i << ") left: " << leftPoints[i] << endl;
        cout << "   right: " << rightPoints[i] << endl;
    }
}

void Branch::addPoint(Point p)
{
    auto tempP = p - getHead();
    auto phi = tempP.angle();
    auto points = splitPoint(p, phi);
    leftPoints.push_back(points.first);
    rightPoints.push_back(points.second);
}

void Branch::addDPoint(Point p)
{
    auto phi = p.angle();
    auto points = splitPoint(p, phi);
    leftPoints.push_back(points.first);
    rightPoints.push_back(points.second);
}

void Branch::addPolar(Polar p)
{ 
    auto newPoint = Point{p}; 
    newPoint += getHead();
    auto points = splitPoint(newPoint, p.phi);
    leftPoints.push_back(points.first);
    rightPoints.push_back(points.second);
}

void Branch::removeHeadPoint()
{
    leftPoints.pop_back();
    rightPoints.pop_back();
}

double Branch::width()
{
    return eps;
}

void Branch::setWidth(double epsVal)
{
    eps/*of object*/ = epsVal/*passed argument*/;
}

Point Branch::getHead()
{
    auto lastIndex = leftPoints.size() - 1;
    return (leftPoints[lastIndex] + rightPoints[lastIndex]) / 2;
}

double Branch::getHeadAngle()
{   
    auto index = leftPoints.size() - 1;
    auto point = leftPoints[index] - leftPoints[index - 1];
    
    return point.angle();
}

Point Branch::getTail()
{
    return mergePoints(leftPoints[0], rightPoints[0]);
}

bool Branch::empty()
{
    return leftPoints.empty();
}

double Branch::length()
{   
    double len = 0;
    //TODO: Test it
    for(unsigned int i = 1; i < leftPoints.size(); ++i)
    {
        auto p = leftPoints[i] - leftPoints[i - 1];
        len += p.norm(); 
    }
    return len;
}

unsigned int Branch::size()
{
    return leftPoints.size();
}

double Branch::averageSpeed()
{
    return length()/size();
}








/*
    Geometry object

*/

void Geometry::SetSquareBoundary(
    Point BottomBoxCorner, 
    Point TopBoxCorner, 
    double dx)
{
    boundaryPoints = {
        {BottomBoxCorner.x + (dx + eps / 2), BottomBoxCorner.y},   //node 2
        {TopBoxCorner.x,                 BottomBoxCorner.y},       //node 3
        {TopBoxCorner.x,                 TopBoxCorner.y},          //node 4
        {BottomBoxCorner.x,              TopBoxCorner.y},          //node 5
        {BottomBoxCorner.x,              BottomBoxCorner.y},       //node 6
        {BottomBoxCorner.x + (dx - eps / 2), BottomBoxCorner.y}    //node 7
    };

    boundaryLines = {
        {1, 2, Markers::Bottom},   
        {2, 3, Markers::Right},   
        {3, 4, Markers::Top},   
        {4, 5, Markers::Left}, 
        {5, 6, Markers::Bottom}
        };
}

void Geometry::addPoints(vector<Point> points)
{
    
}

void Geometry::addDPoints(vector<Point> shifts)
{
    
}

void Geometry::initiateRootBranch(unsigned int id)
{
    auto [boundaryEndPoint, phi] = GetEndPointOfSquareBoundary();
    rootBranchId = id;
    branches.push_back(Branch(rootBranchId, boundaryEndPoint, phi));
    branchIndexes[id] = branches.size() - 1;
}

void Geometry::generateCircularBoundary()
{
    unsigned int curId = rootBranchId;
    for(auto &el : boundaryPoints)
        cout << el << endl;
    //inserting boundary conditions
    points.insert(points.end(), boundaryPoints.begin(), boundaryPoints.end());

    if(branchIndexes.count(curId))
        InserBranchTree(curId);

}
/*
    Recursive inserting of branches
*/
void Geometry::InserBranchTree(unsigned int id)
{
    auto curBranch = branches[branchIndexes[id]];
    curBranch.print();
    if(curBranch.size() > 1){

        points.insert(end(points), 
            begin(curBranch.leftPoints) + 1, 
            end(curBranch.leftPoints) - 1
            );

        if(branchRelation.count(id))
        {
            auto leftId = branchRelation[id].first;
            auto rightId = branchRelation[id].second;
            InserBranchTree(leftId);
            points.push_back(curBranch.getHead());
            InserBranchTree(rightId);
        }
        else
            //inserting narrow tip
            points.push_back(curBranch.getHead());

        //inserting right branch side in reverse order
        points.insert(
            points.end(), 
            curBranch.rightPoints.rbegin() + 1, 
            curBranch.rightPoints.rend() - 1);
    }
}

void Geometry::addPolar(Polar p, bool bRelativeAngle)
{
    if (branchRelation.count(p.index))
        throw std::invalid_argument("branch already has ancestors!");
    if (!branchIndexes.count(p.index))
        throw std::invalid_argument("Such branch does not exist");

    auto & curBranch = branches[branchIndexes[p.index]];
    curBranch.addPolar(p);
}

pair<Point, double> Geometry::GetEndPointOfSquareBoundary()
{
    auto x = (boundaryPoints[0].x + boundaryPoints.back().x) / 2;
    auto y = boundaryPoints[0].y;
    return {Point{x, y}, M_PI / 2.};
}

void Geometry::SetEps(double epsVal){
    eps/*object parameter*/ = epsVal/*argument*/;
}

Branch& Geometry::GetBranch(unsigned int id)
{
    return branches[branchIndexes[id]];
}

}