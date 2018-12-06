
package ops.simulations;

import ops.*;

network RRS
{
    parameters:
        int numNodes = default(20);

    submodules:      
        node[numNodes]: KPromoteNode {
            nodeID = index;
            promoteAppDeployed = 1;
        }
}

