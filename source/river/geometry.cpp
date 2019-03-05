#include "geometry.hpp"


namespace River{

/*
    GeomPolar Class

*/
GeomPolar::GeomPolar(double r, double phiVal, 
      int branchIdVal, int regionTagVal,
      double meshSizeVal):
      branchId(branchIdVal),
      regionTag(regionTagVal),
      meshSize(meshSizeVal)
{
    dl = r;
    phi = phiVal;
}


/*
    GeomLine Class

*/
GeomLine::GeomLine(unsigned int p1Val, unsigned int p2Val, 
    int branchIdVal, int regionTagVal):
    p1(p1Val), p2(p2Val),
    branchId(branchIdVal), regionTag(regionTagVal)
{}



/*
    GeomPoint Class 

*/

    GeomPoint::GeomPoint(double xval, double yval, 
            int branchIdVal, int regionTagVal, double msize):
    x(xval),y(yval),
    regionTag(regionTagVal), branchId(branchIdVal),
    meshSize(msize)
    {}

    GeomPoint::GeomPoint(const GeomPolar &p)
    {
      x = p.dl * cos(p.phi);
      y = p.dl * sin(p.phi);
      regionTag = p.regionTag;
      branchId = p.branchId;
      meshSize = p.meshSize;
    }

    GeomPoint& GeomPoint::rotate(double phi)
    {
        auto tempx = x,
            tempy = y;
        x = tempx*cos(phi) - tempy*sin(phi);
        y = tempx*sin(phi) + tempy*cos(phi);
        
        return *this;
    }

    double GeomPoint::norm() const
    {
      return sqrt(x*x + y*y);
    }

    GeomPoint GeomPoint::getNormalized()
    {
        auto n = norm();
        if(n == 0.)
            throw std::invalid_argument( "norm is equal to zero");
        return GeomPoint{x / n, y / n, branchId, regionTag, meshSize};
    }

    GeomPolar GeomPoint::getPolar() const
    {
        return GeomPolar{norm(), angle(), branchId, regionTag, meshSize};
    }

    GeomPoint& GeomPoint::normalize()
    {
        auto l = norm();
        if(l == 0.)
            throw std::invalid_argument( "norm is equal to zero");
        x /= l;
        y /= l;

        return *this;
    }

    double GeomPoint::angle() const
    {
        auto n = norm();
        if(n == 0.)
            throw std::invalid_argument( "norm is equal to zero");
        double phi = acos(x / n);
        if(y < 0)
            phi = -phi;
        return phi;
    }

    double GeomPoint::angle(GeomPoint &p) const
    {
        auto n = norm();
        auto pn = p.norm();
        if(n == 0. || pn == 0.)
            throw std::invalid_argument("norm of one or another vector is 0");

        //order of points is important
        double phi = acos((x*p.x + y*p.y)/n/pn);
        double sign = x*p.y - p.x*y > 0 ? 1 : -1;//FIXME: is this sign correct?
        phi *= sign;
        return phi;
    }
    GeomPoint& GeomPoint::print()
    {
        cout << *this << endl;
        return *this;
    }

    bool GeomPoint::operator==(const GeomPoint& p) const
    {   
        return (abs(x - p.x)) < eps && (abs(y - p.y) < eps);
    }

    GeomPoint GeomPoint::operator+(const GeomPoint& p) const
    { 
        return GeomPoint{x + p.x, y + p.y, branchId, regionTag, meshSize};
    }

    GeomPoint& GeomPoint::operator+=(const GeomPoint& p) 
    { 
        x += p.x;
        y += p.y; 
        return *this; 
    }

    GeomPoint GeomPoint::operator-(const GeomPoint& p) const
    { 
        return GeomPoint{x - p.x, y - p.y, branchId, regionTag, meshSize};
    }

    GeomPoint& GeomPoint::operator-=(const GeomPoint& p) 
    { 
        x -= p.x;
        y -= p.y; 
        return *this; 
    }
    
    double GeomPoint::operator*(const GeomPoint& p) const
    { 
        return (x*p.x + y*p.y); 
    }

    GeomPoint GeomPoint::operator*(const double gain) const
    { 
        return GeomPoint{x*gain, y*gain, branchId, regionTag, meshSize};
    }

    GeomPoint& GeomPoint::operator*=(const double gain)
    { 
        x *= gain;
        y *= gain;
        return *this;
    }

    GeomPoint GeomPoint::operator/(const double gain) const
    { 
        return GeomPoint{x/gain, y/gain, branchId, regionTag, meshSize};
    }

    GeomPoint& GeomPoint::operator/=(const double gain)
    { 
        x /= gain;
        y /= gain;
        return *this;
    }

    double GeomPoint::operator[](const int index) const
    {
        if(index == 0)
            return x;
        else if(index == 1)
            return y;
        else
            throw std::invalid_argument( "index should be 0 or 1");
    }

    ostream& operator << (ostream& write, const GeomPoint& p)
    {
        write << "point:  {" << p.x << ", " << p.y << "} " << endl 
              << "        id: " << p.branchId << ", tag: " << p.regionTag << endl
              << "        mesh size: " << p.meshSize ;
        return write;
    }



/*
 
    Branch object

*/


Branch::Branch(unsigned long int id, GeomPoint sourcePoint, double phi, double epsVal):
id(id), 
tailAngle(phi),
eps(epsVal)
{
    sourcePoint.branchId = id;
    auto points = splitPoint(sourcePoint, phi);
    leftPoints.push_back(points.first);
    rightPoints.push_back(points.second);
}

pair<GeomPoint, GeomPoint> Branch::splitPoint(GeomPoint p, double phi)
{
    GeomPoint pLeft{p}, pRight{p};
    pLeft  += GeomPoint{0, 1}.rotate(phi) * eps/2;
    pRight += GeomPoint{0, -1}.rotate(phi) * eps/2;
    return {pLeft, pRight};
}

GeomPoint Branch::mergePoints(GeomPoint p1, GeomPoint p2)
{
    return (p1 + p2)/2;
}

void Branch::print()
{
    for(unsigned int i = 0; i < leftPoints.size(); ++i )
    {
        cout << "Branch ID : " << leftPoints[0].branchId << endl;
        cout << "size: " << leftPoints.size() << endl;
        cout << i << ") left: " << leftPoints[i] << endl;
        cout << "   right: " << rightPoints[i] << endl;
    }
}

void Branch::addPoint(GeomPoint p)
{
    auto tempP = p - getHead();
    auto phi = tempP.angle();
    auto points = splitPoint(p, phi);
    leftPoints.push_back(points.first);
    rightPoints.push_back(points.second);
}

void Branch::addDPoint(GeomPoint p)
{
    auto phi = p.angle();
    auto points = splitPoint(p, phi);
    leftPoints.push_back(points.first);
    rightPoints.push_back(points.second);
}

void Branch::addPolar(GeomPolar p, bool bRelativeAngle)
{ 
    
    if(bRelativeAngle)
        p.phi += getHeadAngle();

    auto newPoint = GeomPoint(p);
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

GeomPoint Branch::getHead()
{
    auto lastIndex = size() - 1;
    return (leftPoints[lastIndex] + rightPoints[lastIndex]) / 2;
}

double Branch::getHeadAngle()
{   
    cout << "getHeadAngle" << endl;
    double angle = tailAngle;
    
    if(auto index = size() - 1; index > 0)
    {
        auto point = leftPoints[index] - leftPoints[index - 1];
        cout << point << endl;
        angle = point.angle();
    }
    
    return angle;
}

double Branch::getTailAngle()
{   
    auto point = leftPoints[1] - leftPoints[0];
    return point.angle();
}

GeomPoint Branch::getTail()
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
    GeomPoint BottomBoxCorner, 
    GeomPoint TopBoxCorner, 
    double dx)
{
    boundaryPoints = {
        {BottomBoxCorner.x + (dx + eps / 2), BottomBoxCorner.y, 
        0, 0, riverMeshSize},        //node 1
        {TopBoxCorner.x,                     BottomBoxCorner.y, 
        0, 0, boundariesMeshSize},   //node 2
        {TopBoxCorner.x,                     TopBoxCorner.y, 
        0, 0, boundariesMeshSize},   //node 3
        {BottomBoxCorner.x,                  TopBoxCorner.y, 
        0, 0, boundariesMeshSize},   //node 4
        {BottomBoxCorner.x,                  BottomBoxCorner.y, 
        0, 0, boundariesMeshSize},   //node 5
        {BottomBoxCorner.x + (dx - eps / 2), BottomBoxCorner.y, 
        0, 0, riverMeshSize}         //node 6
    };

    int borderBranchId = 0;
    boundaryLines = {
        {0, 1, borderBranchId, Markers::Bottom},   
        {1, 2, borderBranchId, Markers::Right},   
        {2, 3, borderBranchId, Markers::Top},   
        {3, 4, borderBranchId, Markers::Left}, 
        {4, 5, borderBranchId, Markers::Bottom}
        };
}

void Geometry::addPoints(vector<GeomPoint> points)
{
    
}

void Geometry::addDPoints(vector<GeomPoint> shifts)
{
    
}

Branch& Geometry::initiateRootBranch(unsigned int id)
{
    auto [boundaryEndPoint, phi] = GetEndPointOfSquareBoundary();
    
    rootBranchId = id;
    auto rootBranch = Branch(rootBranchId, boundaryEndPoint, phi, eps);

    branches.push_back(rootBranch);
    branchIndexes[id] = branches.size() - 1;

    return rootBranch;
}

GeomPoint Geometry::mergedLeft(double phi)
{
    auto vec = GeomPoint{-eps/2*tan(bifAngle/2), eps/2, 0, 0, riverMeshSize};
    return vec.rotate(phi);
}

GeomPoint Geometry::mergedRight(double phi)
{
    auto vec = GeomPoint{-eps/2*tan(bifAngle/2), - eps/2, 0, 0, riverMeshSize};
    return vec.rotate(phi);
}

GeomPoint Geometry::mergedCenter(double phi)
{
    auto vec = GeomPoint{eps/2/sin(bifAngle), 0, 0, 0, riverMeshSize};
    return vec.rotate(phi);
}

void Geometry::generateCircularBoundary()
{
    unsigned int curId = rootBranchId;

    //inserting boundary conditions
    points.insert(points.end(), boundaryPoints.begin(), boundaryPoints.end());

    if(branchIndexes.count(curId))
        InserBranchTree(curId, M_PI/2., true);

    lines.insert(lines.end(), boundaryLines.begin(), boundaryLines.end());
    int index = boundaryPoints.size() - 1;
    for(auto &p: points)
    {
        if(index < points.size() - 1)
            lines.push_back(GeomLine(index, index + 1, p.branchId, Markers::River));
        else
        {
            lines.push_back(GeomLine(index, 0, p.branchId, Markers::River));
            break;
        }

        index++;
    }
}


 
void Geometry::InitiateMesh(tethex::Mesh &meshio)
{   
    generateCircularBoundary();
    
    meshio.vertices.reserve(points.size());
    meshio.lines.reserve(points.size());//same length as points cos circular region

    for(GeomPoint p: points)
        meshio.vertices.push_back(
            tethex::Point(p.x, p.y, 0/*z-coord*/, p.regionTag, p.meshSize));

    for(auto &l: lines)
        meshio.lines.push_back(new tethex::Line(l.p1, l.p2, l.regionTag));
}
/*
    Recursive inserting of branches
*/
void Geometry::InserBranchTree(unsigned int id, double phi, bool isRoot)
{
    auto curBranch = get_branch(id);
    phi = curBranch->getHeadAngle();
    if(curBranch->size() > 1)
    {
        points.insert(end(points), 
            begin(curBranch->leftPoints) + 1, 
            end(curBranch->leftPoints) - 1
            );
        

        if(branchRelation.count(id))
        {
            auto leftId = branchRelation[id].first;
            auto rightId = branchRelation[id].second;

            //Left merged point
            auto leftMergPoint = curBranch->getHead() + mergedLeft(phi);
            //leftMergPoint.meshSize = riverMeshSize;
            points.push_back(leftMergPoint);

            InserBranchTree(leftId, curBranch->getHeadAngle());
            
            //Tip point
            auto tipPoint = curBranch->getHead() + mergedCenter(phi);
            tipPoint.meshSize = riverMeshSize;
            points.push_back(tipPoint);

            InserBranchTree(rightId, curBranch->getHeadAngle());

            //Left merged point
            auto rightMergPoint = curBranch->getHead() + mergedRight(phi);
            //rightMergPoint.meshSize = riverMeshSize;
            points.push_back(rightMergPoint);
        }
        else
        {
            //inserting narrow tip
            auto p = curBranch->getHead();
            p.meshSize = tipMeshSize;
            points.push_back(p);
        }
        

        //inserting right branch side in reverse order
        points.insert(
            points.end(), 
            curBranch->rightPoints.rbegin() + 1, 
            curBranch->rightPoints.rend() - 1);
        
    }
}

void Geometry::addPolar(GeomPolar p, bool bRelativeAngle)
{   
    if (q_branch_childs(p.branchId))
        throw std::invalid_argument("branch already has ancestors!");

    auto curBranch = get_branch(p.branchId);
    p.meshSize = riverMeshSize;
    curBranch->addPolar(p, bRelativeAngle/*relative angle*/);
}

pair<GeomPoint, double> Geometry::GetEndPointOfSquareBoundary()
{
    auto x = (boundaryPoints[0].x + boundaryPoints.back().x) / 2;
    auto y = boundaryPoints[0].y;
    return {GeomPoint{x, y}, M_PI / 2.};
}


void Geometry::SetEps(double epsVal){
    eps/*object parameter*/ = epsVal/*argument*/;
}

void Geometry::clear()
{
    points.clear();
    lines.clear();
}


unsigned int Geometry::generateID(unsigned int prevID, bool isRight)
{
    return (prevID << 1) + (int)isRight;
}

pair<unsigned int, unsigned int> Geometry::AddBiffurcation(unsigned int id, double dl)
{
    if (branchRelation.count(id))
        throw std::invalid_argument("branch already has ancestors!");
    if(!branchIndexes.count(id))
        throw std::invalid_argument("Such branch does not exist");
    
    auto leftId = generateID(id),
        rightId = generateID(id, true/*is right branch*/);

    //Some values from origin brnach
    auto originBranch = get_branch(id);
    auto headPoint = originBranch->getHead();
    headPoint.meshSize = riverMeshSize;
    auto phi = originBranch->getHeadAngle();

    add_branch_relation(id, leftId, rightId);

    //setting left branch
    auto leftBranch = Branch(leftId, headPoint, phi + bifAngle, eps);
    leftBranch.addPolar({dl, 0, headPoint.branchId, headPoint.regionTag, riverMeshSize}/*relative coords*/);
    add_branch(leftBranch, leftId);

    //setting right branch
    auto rightBranch = Branch(rightId, headPoint, phi - bifAngle, eps);
    rightBranch.addPolar({dl, 0, headPoint.branchId, headPoint.regionTag, riverMeshSize}/*relative coord*/);
    add_branch(rightBranch, rightId);


    return {leftId, rightId};
}

//Recursively generates array of branch ids which at the end of tree(don't have childs)
void Geometry::generate_ids_of_tip_branches(vector<int> &IdsOfBranchesAtTip, int branchId)
{
    if(!q_branch_childs(branchId))
        IdsOfBranchesAtTip.push_back(branchId);
    else
    {
        auto[leftBranchId, rightBranchId] = get_branch_childs(branchId);
        generate_ids_of_tip_branches(IdsOfBranchesAtTip, leftBranchId);
        generate_ids_of_tip_branches(IdsOfBranchesAtTip, rightBranchId);
    }

}

//returns array of points at tips of branches
vector<pair<GeomPoint, double>> Geometry::GetTipPoints()
{
    vector<int> IdsOfBranchesAtTip;
    generate_ids_of_tip_branches(IdsOfBranchesAtTip, rootBranchId);

    vector<pair<GeomPoint, double>> tipPointsAndAngles;
    for(auto branchId: IdsOfBranchesAtTip)
    {
        Branch* curBranch = get_branch(branchId);
        auto point = curBranch->getHead();
        auto angle = curBranch->getHeadAngle();
        tipPointsAndAngles.push_back(make_pair(point, angle));

    }

    return tipPointsAndAngles;
}


void Geometry::add_branch(Branch& branch, int id)
{
    if(q_branch(id))
        throw invalid_argument("Can't add branch. Such branch already exist");

    branches.push_back(branch);
    branchIndexes[id] = branches.size() - 1;
}


Branch* Geometry::get_branch(int id)
{
    if (!branchIndexes.count(id))
        throw invalid_argument("Such brunch with id doesn't exist");

    return &(branches[branchIndexes[id]]);
}

bool Geometry::q_branch(int id)
{
    return branchIndexes.count(id);
}


bool Geometry::q_branch_childs(int id)
{
    return branchRelation.count(id);
}


pair<int, int> Geometry::get_branch_childs(int id)
{
    if (!branchRelation.count(id))
        throw invalid_argument("Such brunch with id doesn't exist or it has no childs");

    return branchRelation[id];
}

void Geometry::add_branch_relation(int baseId, int leftId, int rightId)
{
    if(q_branch_childs(baseId))
        throw invalid_argument("Current base branch already has childs!");

    branchRelation[baseId] = {leftId, rightId};   
}




}