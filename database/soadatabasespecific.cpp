#include "database/soadatabase.h"
#include "soalogger.h"



SoaRw *SoaDb::DeployApplicationVersion(unsigned int appVerId, unsigned int servDomId, bool deployServices) {

	SoaTb *apps = Tb("ApplicationVersion"), *domains = Tb("ServiceDomain");
	SoaRw *app = apps->RwByViewId(appVerId), *domain = domains->RwByViewId(servDomId);

	if (app && domain) {

		// create deployed application

		SoaTb *depApps = Tb("DeployedApplication");

		SoaRw *depApp = depApps->AllocRw(depApps->GetAvailableId(), false);
		depApp->SetCe("ApplicationVersion", app);
		depApp->SetCe("ServiceDomain", domain);
		depApps->AddRw(depApp);

		if (deployServices) {

			// find default intermediary

			SoaRw *intermediary = 0;
			const SoaRwList *ints = domain->Children("Intermediary");
			for (SoaRwList::const_iterator i = ints->begin(); i != ints->end(); ++i)
				if ((*i)->Name() == DEFAULT_INTERMEDIARY_NAME) {
					intermediary = *i;
					break;
				}

			if (intermediary) {

				// create deployed services

				SoaTb *depServs = Tb("DeployedService");
				SoaTb *endpoints = Tb("Endpoint");

				const SoaRwList *servs = app->Children("ServiceVersion");
				for (SoaRwList::const_iterator serv = servs->begin(); serv != servs->end(); ++serv) {

					SoaRw *depServ = depServs->Search().
							Wt("ServiceVersion", *serv).
							Wt("DeployedApplication", depApp).
							Get(depServs, true);

					endpoints->Search().
							Wt("intermediaryGivenName", DEFAULT_ENDPOINT_NAME).
							Wt("ServiceDomain", domain).
							Wt("Intermediary", intermediary).
							Wt("DeployedService", depServ).
							Get(endpoints, true);
				}
			}
			else
				sl::log << "DeployApplicationVersion error: default intermediary not found" << sl::end;
		}
	}
	else
		sl::log << "DeployApplicationVersion error: application version or service domain missing" << sl::end;

	return 0;
}
