#include "widgets/wdggateway.h"
#include "interfaces/iviewable.h"
#include "soawidget.h"



// Conflates db and gt domain-services-endpoint trees
// and marks each node as either 'old', 'new' or 'deleted'
void WdgGateway::Conflate() {

	GtDomain dbD = wg->Db().Services(wg->Db().SelectedDomain());

	// cross-reference db & gateway services & endpoints

	int sIndex = 0;
	for (GtDomain::iterator gtS = conflation.begin(); gtS != conflation.end(); ++gtS) {

		GtDomain::iterator dbS = dbD.Find(gtS->name);

		if (dbS != dbD.end()) { // gateway service found in db

			// copy service data (log if anything changed)

			// ...

			gtS->viewId = dbS->viewId;
			gtS->selected = dbS->selected;

			int eIndex = 0;
			for (GtService::iterator gtE = gtS->begin(); gtE != gtS->end(); ++gtE) {

				GtService::iterator dbE = dbS->Find(gtE->intGivenId);

				if (dbE != dbS->end()) {

					// copy endpoint data (log if anything changed)

					// ...

					gtE->viewId = dbE->viewId;
					gtE->selected = dbE->selected;

					dbS->erase(dbE);
					gtE->status = gesOld;
				}
				else {

					// new endpoint for the deployed service (can add automatically)

					// ...

					gtE->status = gesNew;
				}

				gtE->SetIndices(sIndex, eIndex);
				++eIndex;
			}

			// all the leftover endpoints in the db service are to be deleted (not found in gateway)

			for (GtService::iterator dbE = dbS->begin(); dbE != dbS->end(); ++dbE) {

				dbE->SetIndices(sIndex, eIndex);
				dbE->status = gesDeleted;
				gtS->push_back(*dbE);

				++eIndex;
			}

			// erase service from db services if matched gateway service

			dbD.erase(dbS);
			gtS->status = gesOld;
		}
		else {	// gateway service not found in db (new)

			// ...

			int eIndex = 0;
			for (GtService::iterator gtE = gtS->begin(); gtE != gtS->end(); ++gtE) {
				gtE->SetIndices(sIndex, eIndex);
				gtE->status = gesNew;
			}

			gtS->status = gesNew;
		}

		gtS->SetIndices(sIndex);
		++sIndex;
	}

	// all the leftover services in db domain are to be deleted (not found in gateway)

	for (GtDomain::iterator dbS = dbD.begin(); dbS != dbD.end(); ++dbS) {

		dbS->SetIndices(sIndex);
		dbS->status = gesDeleted;
		conflation.push_back(*dbS);

		++sIndex;
	}
}
